#include "vera/app.h"

class MainApp : public vera::App {

    Gsplat plant;
    bool   bDebug = false;
    bool   bGrid = false;
    bool   bTarget = false;

    void setup() {
        background(0.1f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 2.5f));
        camera()->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

        plant.load("flower.splat");
    }

    void update() {
    }

    void draw() {
        orbitControl();

        blendMode(BLEND_ALPHA);
        setDepthTest(false);
        
        push();
        // rotateY( radians(millis() * 0.025f) );

        fill(1.0f);
        model( plant );

        if (bDebug) {
            noFill();
            model( plant );
        }

        pop();

        if (bGrid) {
            // Draw grid
            vera::noFill();
            vera::stroke(0.3f);
            vera::strokeWeight(1.0f);
            for (int i = -10; i <= 10; i++) {
                vera::line(glm::vec3(-10.0f, 0.0f, (float)i), glm::vec3(10.0f, 0.0f, (float)i));
                vera::line(glm::vec3((float)i, 0.0f, -10.0f), glm::vec3((float)i, 0.0f, 10.0f));
            }
        }

        if (bTarget) {
            // Draw a X Y Z axis at the camera target
            glm::vec3 target = camera()->getTarget();
            float axisLength = 0.2f;
            noFill();
            vera::strokeWeight(1.0f);
            // X - Red
            vera::stroke(1.0f, 0.0f, 0.0f);
            vera::line(target, target + glm::vec3(axisLength, 0.0f, 0.0f));
            // Y - Green
            vera::stroke(0.0f, 1.0f, 0.0f);
            vera::line(target, target + glm::vec3(0.0f, axisLength, 0.0f));
            // Z - Blue
            vera::stroke(0.0f, 0.0f, 1.0f);
            vera::line(target, target + glm::vec3(0.0f, 0.0f, axisLength));
        }
    }

    void onWindowResize(int _width, int _height) {
        camera()->setViewport(_width, _height);
    }

    void onKeyPress(int key){
        float moveSpeed = 0.1f;

        if (key == 'd') {
            bDebug = !bDebug;
        }
        else if (key == 'g') {
            bGrid = !bGrid;
        }
        else if (key == 't') {
            bTarget = !bTarget;
        }

        // change grid dimension
        else if (key == 'w') {
            plant.setGridDim( plant.getGridDim() + 1);
        } else if (key == 's') {
            plant.setGridDim( plant.getGridDim() - 1);
        } 
        
        // change occlusion threshold
        else if (key == 'r') {
            plant.setOcclusionThreshold( plant.getOcclusionThreshold() + 1);
            std::cout << "Occlusion Threshold: " << plant.getOcclusionThreshold() << std::endl;
        } else if (key == 'f') {
            plant.setOcclusionThreshold( std::max(0, plant.getOcclusionThreshold() - 1) );
            std::cout << "Occlusion Threshold: " << plant.getOcclusionThreshold() << std::endl;
        }

        // change occlusion scale
        else if (key == 'z') {
            plant.setOcclusionScale( plant.getOcclusionScale() + 0.05f);
            std::cout << "Occlusion Scale: " << plant.getOcclusionScale() << std::endl;
        } else if (key == 'x') {
            plant.setOcclusionScale( std::max(0.0f, plant.getOcclusionScale() - 0.05f) );
            std::cout << "Occlusion Scale: " << plant.getOcclusionScale() << std::endl;
        }

        // 
        // Basic camera controls 
        else if (key == VERA_KEY_LEFT) {
            camera()->move(-moveSpeed, 0.0f, 0.0f);
        } else if (key == VERA_KEY_RIGHT) {
            camera()->move(moveSpeed, 0.0f, 0.0f);
        } else if (key == VERA_KEY_UP) {
            camera()->move(0.0f, 0.0f, moveSpeed);
        } else if (key == VERA_KEY_DOWN) {
            camera()->move(0.0f, 0.0f, -moveSpeed);
        }
    }

};

MainApp app;

int main(int argc, char **argv) {
    
    vera::WindowProperties prop;
    prop.screen_width = 1080/2;
    prop.screen_height = 1920/2;

    #if !defined(__EMSCRIPTEN__) && !defined(PLATFORM_RPI)
    prop.msaa = 4;
    #endif

    app.run(prop);

    return 1;
}
