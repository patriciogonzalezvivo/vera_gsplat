#include "vera/app.h"
#include "vera/types/gsplat.h"

class MainApp : public vera::App {

    Gsplat gsplat;

    void setup() {
        background(0.1f);
        
        camera()->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        camera()->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));

        gsplat.load("../cactus.ply");
        // gsplat.load("../flower.splat");
        // gsplat.load("../bonsai.splat");
    }

    void update() {
    }

    void onWindowResize(int _width, int _height) {
        camera()->setViewport(_width, _height);
    }

    void draw() {
        orbitControl();

        blendMode(BLEND_ALPHA);
        // setDepthTest(false);
        
        push();
        rotateX(-3.14f);
        rotateY(time * 0.1f);

        gsplat.draw(*camera(), worldMatrix());

        // setDepthTest(true);

        // draw grid
        translate(0.0f, 1.0f, 0.0f);
        stroke(0.5f);
        noFill();
        for (int i = -5; i <= 5; i++) {
            line( glm::vec3(-5.0f, 0.0f, (float)i), glm::vec3(5.0f, 0.0f, (float)i) );
            line( glm::vec3((float)i, 0.0f, -5.0f), glm::vec3((float)i, 0.0f, 5.0f) );
        }

        pop();
    }
};

int main(int argc, char **argv) {
    MainApp app;
    app.run();
    return 0;
}
