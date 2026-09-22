#include <demo/demo.hpp>

class CircleDemo : public demo::Demo {
public:
    const char* title() const override { return "Circle Demo"; };

    demo::Vec2 center;
    bool big = false;

    void update(float dt, const demo::InputData& input) override {
        center = input.pointer;
        big = input.pointer_held;
    }

    void draw(demo::Canvas& canvas) override {
        canvas.fill({ 20, 20, 20, 255 });
        canvas.clear();
        canvas.fill({ 255, 255, 255, 255 });
        canvas.circle(center, 60 + ((big) ? 30 : 0));
    }
};

std::unique_ptr<demo::Demo> demo::create_demo() {
    return std::make_unique<CircleDemo>();
}