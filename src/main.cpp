#include "vera/app.h"
#include "vera/types/gsplat.h"
#include "vera/ops/fs.h"

class MainApp : public vera::App {

    Gsplat gsplat;

    // Buffers
    GLuint vao;
    GLuint positionVBO;
    GLuint indexVBO;

    // Cached attribute locations
    GLint a_position = -1;
    GLint a_index = -1;

    std::vector<uint32_t> depthIndex;

    void setup() {
        background(0.0f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        camera()->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

        gsplat.load("../cactus.ply");
        texture(gsplat.getTexture(), "u_tex0");

        addShader("splat_shader", loadGlslFrom("shaders/splat.frag"), loadGlslFrom("shaders/splat.vert"));

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
        
        a_position = glGetAttribLocation(shader("splat_shader")->getProgram(), "a_position");
        glEnableVertexAttribArray(a_position);
        glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
        // Index buffer (per-instance)
        glGenBuffers(1, &indexVBO);
        
        glBindVertexArray(0);
    }

    void update() {
    }

    void draw() {
        orbitControl();

        // Sort splats by depth
        glm::mat4 viewProj = camera()->getProjectionMatrix() * camera()->getViewMatrix();
        gsplat.sort(viewProj, depthIndex);

        // Upload sorted indices
        glBindBuffer(GL_ARRAY_BUFFER, indexVBO);
        glBufferData(GL_ARRAY_BUFFER, depthIndex.size() * sizeof(uint32_t), 
                    depthIndex.data(), GL_STREAM_DRAW);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

        // glUseProgram(shader("splat_shader")->getProgram());
        Shader* splat_shader = shader("splat_shader");
        splat_shader->use();

        glBindVertexArray(vao);

        // Set uniforms
        splat_shader->setUniformTexture("u_tex0", texture("u_tex0")); // Texture unit 0
        splat_shader->setUniform("u_tex0Resolution", glm::vec2(texture("u_tex0")->getWidth(), texture("u_tex0")->getHeight()));
        splat_shader->setUniform("u_projectionMatrix", camera()->getProjectionMatrix());
        splat_shader->setUniform("u_viewMatrix", camera()->getViewMatrix());
        splat_shader->setUniform("u_resolution", glm::vec2(width, height));
        
        // Compute focal lengths from FOV
        float fovRad = glm::radians(camera()->getFOV());
        float fy = height / (2.0f * std::tan(fovRad / 2.0f));
        splat_shader->setUniform("u_focal", glm::vec2(fy, fy));
        
        // Setup vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glEnableVertexAttribArray(a_position);
        glVertexAttribPointer(a_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, indexVBO);
        if (a_index == -1) {
            a_index = glGetAttribLocation(shader("splat_shader")->getProgram(), "a_index");
        }
        glEnableVertexAttribArray(a_index);
        glVertexAttribIPointer(a_index, 1, GL_UNSIGNED_INT, 0, 0);
        glVertexAttribDivisor(a_index, 1);

        
        // Draw
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, gsplat.count());
        
        glBindVertexArray(0);
    }
};

int main(int argc, char **argv) {
    MainApp app;
    app.run();
    return 1;
}
