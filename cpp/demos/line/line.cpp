#include <demo/demo.hpp>

class LineDemo : public demo::Demo {
public:
    const char* title() const override { return "Line Demo"; };

    demo::Vec2 head;
    demo::Vec2 tail = {0, 100};

    void update(float dt, const demo::InputData& input) override {
        tail = input.pointer;
    }

    void draw(demo::Canvas& canvas) override {
        canvas.fill({ 20, 20, 20, 255 });
        canvas.clear();
        canvas.stroke({ 255, 255, 255, 255 });
        canvas.line(head, tail);
    }
};

std::unique_ptr<demo::Demo> demo::create_demo() {
    return std::make_unique<LineDemo>();
}