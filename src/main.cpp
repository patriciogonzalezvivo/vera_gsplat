#include "vera/app.h"
#include "vera/types/gsplat.h"

class MainApp : public vera::App {

    Gsplat plant;

    void setup() {
        background(0.1f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        camera()->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));

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
        rotateY( radians(millis() * 0.025f) );

        model( plant );

        pop();
    }

    void onWindowResize(int _width, int _height) {
        camera()->setViewport(_width, _height);
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
