#include "vera/app.h"
#include "vera/types/gsplat.h"

class MainApp : public vera::App {

    Gsplat gsplat;

    void setup() {
        background(0.0f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        camera()->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));

        gsplat.load("../cactus.ply");
    }

    void update() {
    }

    void onWindowResize(int _width, int _height) {
        camera()->setViewport(_width, _height);
    }

    void draw() {
        orbitControl();

        blendMode(BLEND_ALPHA);
        setDepthTest(false);
        
        gsplat.draw(*camera());
    }
};

int main(int argc, char **argv) {
    MainApp app;
    app.run();
    return 0;
}
