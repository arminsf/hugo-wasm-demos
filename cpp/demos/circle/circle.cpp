#include <vector>
#include <cmath>
#include <cstdlib>

#include <demo/demo.hpp>

struct Ball {
    demo::Vec2 pos;
    demo::Vec2 vel; // px/sec
    float radius;

    bool collides(Ball& b) {
        return (pos.x - b.pos.x) * (pos.x - b.pos.x) + (pos.y - b.pos.y) * (pos.y - b.pos.y) < (radius + b.radius) * (radius + b.radius);
    }
};

class CircleDemo : public demo::Demo {
private:
    bool pressed = false;
    
    const float gravity = 300.0;
    const float wall_absorb = 0.8;

    std::vector<Ball> balls;

    void move(float dt) {
        for (Ball& p : balls) {
            p.pos.x += dt * p.vel.x;
            p.pos.y += dt * p.vel.y;
            p.vel.y += dt * gravity;
        }
    }

    void bounce() {
        for (Ball& p : balls) {
            if (p.pos.x < p.radius) {
                p.pos.x = p.radius;
                p.vel.x = std::abs(p.vel.x)*wall_absorb;
            }

            if (p.pos.x > preferred_size().x - p.radius) {
                p.pos.x = preferred_size().x - p.radius;
                p.vel.x = -std::abs(p.vel.x)*wall_absorb;
            }

            if (p.pos.y > preferred_size().y - p.radius) {
                p.pos.y = preferred_size().y - p.radius;
                p.vel.y = -std::abs(p.vel.y)*wall_absorb;
                if (abs(p.vel.y) < 50.0) p.vel.y = 0;
            }
        }

        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                Ball& a = balls[i];
                Ball& b = balls[j];

                float dx = b.pos.x - a.pos.x;
                float dy = b.pos.y - a.pos.y;

                float dist2 = dx * dx + dy * dy;
                float min_dist = a.radius + b.radius;

                if (dist2 >= min_dist * min_dist)
                    continue;

                float dist = std::sqrt(dist2);

                if (dist == 0.0f)
                    continue;

                float nx = dx / dist;
                float ny = dy / dist;

                float rvx = b.vel.x - a.vel.x;
                float rvy = b.vel.y - a.vel.y;

                float velocity_normal = rvx * nx + rvy * ny;

                if (velocity_normal > 0.0f)
                    continue;

                a.vel.x += velocity_normal * nx;
                a.vel.y += velocity_normal * ny;

                b.vel.x -= velocity_normal * nx;
                b.vel.y -= velocity_normal * ny;

                float overlap = min_dist - dist;
                float half_overlap = overlap * 0.5f;

                a.pos.x -= nx * half_overlap;
                a.pos.y -= ny * half_overlap;

                b.pos.x += nx * half_overlap;
                b.pos.y += ny * half_overlap;
            }
        }
    }

public:
    const char* title() const override { return "Circle Demo"; };

    void update(float dt, const demo::InputData& input) override {
        if (input.pointer_held && !pressed) {
            pressed = true;
            balls.push_back(Ball{ input.pointer, demo::Vec2{ 0.0, 0.0 }, float(10 + rand() % 20)  });
        }

        if (!input.pointer_held && pressed)
            pressed = false;

        bounce();
        move(dt);
    }

    void draw(demo::Canvas& canvas) override {
        canvas.fill({ 120, 120, 120, 255 });
        canvas.clear();
        canvas.stroke({ 0, 0, 0, 255 });
        canvas.fill({ 120, 120, 20, 255 });
        canvas.line({0, 0}, preferred_size());
        for (Ball& p : balls) {
            canvas.circle(p.pos, p.radius);
        }
    }
};

std::unique_ptr<demo::Demo> demo::create_demo() {
    return std::make_unique<CircleDemo>();
}