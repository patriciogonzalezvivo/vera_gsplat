#include "vera/app.h"

class MainApp : public vera::App {

    Gsplat plant;
    bool   bDebug = false;

    void setup() {
        background(0.1f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
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
        rotateX(-3.14f);
        // rotateY( radians(millis() * 0.025f) );

        model( plant );

        if (bDebug) {
            plant.renderDebug(*camera(), worldMatrix());
        }

        pop();
    }

    void onWindowResize(int _width, int _height) {
        camera()->setViewport(_width, _height);
    }

    void onKeyPress(int key){
        if (key == 'd') {
            bDebug = !bDebug;
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
