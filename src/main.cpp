#include "vera/app.h"
#include "vera/types/gsplat.h"
#include "vera/ops/fs.h"

class MainApp : public vera::App {

    Gsplat gsplat;
    Texture* splat_texture = nullptr;

    // Buffers
    GLuint vao;
    GLuint positionVBO;
    GLuint indexVBO;

    // Cached attribute locations
    GLint a_position = -1;
    GLint a_index = -1;

    std::vector<uint32_t> depthIndex;
    std::vector<float> indexFloats;

    void setup() {
        background(0.0f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        camera()->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));

        addShader("splat_shader", loadGlslFrom("shaders/splat.frag"), loadGlslFrom("shaders/splat.vert"));
        
        gsplat.load("../cactus.ply");
        splat_texture = gsplat.getTexture();


        // Create VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        // Quad positions for point sprite
        float quadVertices[] = {
            -2.0f, -2.0f,
            2.0f, -2.0f,
            2.0f,  2.0f,
            -2.0f,  2.0f
        };
        
        glGenBuffers(1, &positionVBO);
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        
        Shader* s = shader("splat_shader");
        if (s) {
            a_position = glGetAttribLocation(s->getProgram(), "a_position");
            if (a_position != -1) {
                glEnableVertexAttribArray(a_position);
                glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
            }
        
            // Index buffer (per-instance)
            glGenBuffers(1, &indexVBO);
            glBindBuffer(GL_ARRAY_BUFFER, indexVBO);
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

            a_index = glGetAttribLocation(s->getProgram(), "a_index");
            if (a_index != -1) {
                glEnableVertexAttribArray(a_index);
                // Use glVertexAttribPointer to allow conversion to float in shader
                glVertexAttribPointer(a_index, 1, GL_FLOAT, GL_FALSE, 0, 0); 
                glVertexAttribDivisor(a_index, 1);
            }
        }

        glBindVertexArray(0);
    }

    void update() {
    }

    void draw() {
        orbitControl();

        if (!splat_texture) { return; }

        // Sort splats by depth
        glm::mat4 viewProj = camera()->getProjectionMatrix() * camera()->getViewMatrix();
        gsplat.sort(viewProj, depthIndex);

        // Upload sorted indices
        indexFloats.resize(depthIndex.size());
        for(size_t i = 0; i < depthIndex.size(); i++) {
            indexFloats[i] = (float)depthIndex[i];
        }

        glBindBuffer(GL_ARRAY_BUFFER, indexVBO);
        glBufferData(GL_ARRAY_BUFFER, indexFloats.size() * sizeof(float), indexFloats.data(), GL_STREAM_DRAW);

        blendMode(BLEND_ALPHA);

        Shader* splat_shader = shader("splat_shader");
        if (!splat_shader) {
            return;
        }

        splat_shader->use();

        glBindVertexArray(vao);

        // Set uniforms
        splat_shader->setUniformTexture("u_tex0", splat_texture, 0); // Use member variable directly
        splat_shader->setUniform("u_tex0Resolution", glm::vec2(splat_texture->getWidth(), splat_texture->getHeight()));
        
        // Compute focal lengths from FOV
        float fovRad = glm::radians(camera()->getFOV());
        float fy = height / (2.0f * std::tan(fovRad / 2.0f));
        splat_shader->setUniform("u_focal", glm::vec2(fy, fy));
        
        // Draw
        if (gsplat.count() > 0) {
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, gsplat.count());
        }
        
        glBindVertexArray(0);
    }
};

int main(int argc, char **argv) {
    MainApp app;
    app.run();
    return 1;
}
