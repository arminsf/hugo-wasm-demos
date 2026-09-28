#include <vector>
#include <cmath>
#include <cstdlib>

#include <demo/demo.hpp>

struct Line3 {
    demo::Vec3 a;
    demo::Vec3 b;
};

class EarthSphereDemo : public demo::Demo {
private:
    const int longitude_lines = 12;
    const int latitude_lines = 12;
    demo::Vec3 sphere_center;
    float sphere_radius;
    std::vector<Line3> sphere_lines;

    void create_sphere_lines(float radius, int latitude_lines, int longitude_lines, int circle_sides) {
        sphere_radius = radius;

        // latitude
        for (int i = 1; i <= latitude_lines; i++) {
            const float theta = i * M_PI / (latitude_lines + 1);
            const float circle_radius = radius * sin(theta);
            const float height = radius * cos(theta);
            for (int j = 0; j < circle_sides; j++) {
                const float alpha1 = 2 * j * M_PI / circle_sides;
                const float alpha2 = 2 * (j+1) * M_PI / circle_sides;
                Line3 l;
                l.a.x = circle_radius * sin(alpha1);
                l.a.z = circle_radius * cos(alpha1);
                l.a.y = height;
                l.b.x = circle_radius * sin(alpha2);
                l.b.z = circle_radius * cos(alpha2);
                l.b.y = height;

                sphere_lines.push_back(l);
            }
        }

        std::vector<Line3> great_circle;

        for (int j = 0; j < circle_sides; j++) { 
            const float alpha1 = 2 * j * M_PI / circle_sides;
            const float alpha2 = 2 * (j+1) * M_PI / circle_sides;
            Line3 l;
            l.a.x = 0;
            l.a.y = radius * sin(alpha1);
            l.a.z = radius * cos(alpha1);
            l.b.x = 0;
            l.b.y = radius * sin(alpha2);
            l.b.z = radius * cos(alpha2);

            great_circle.push_back(l);
        }

        for (int i = 0; i < longitude_lines; i++) {
            const float theta = i * M_PI / longitude_lines;
            for (const Line3& l : great_circle) {
                Line3 rotated;
                rotated.a.x = cos(theta) * l.a.x + sin(theta) * l.a.z;
                rotated.a.y = l.a.y;
                rotated.a.z = -sin(theta) * l.a.x + cos(theta) * l.a.z;
                rotated.b.x = cos(theta) * l.b.x + sin(theta) * l.b.z;
                rotated.b.y = l.b.y;
                rotated.b.z = -sin(theta) * l.b.x + cos(theta) * l.b.z;

                sphere_lines.push_back(rotated);
            }
        }
    }

    void translate_sphere(demo::Vec3 v) {
        sphere_center.x += v.x;
        sphere_center.y += v.y;
        sphere_center.z += v.z;
        for (Line3& l : sphere_lines) {
            l.a.x += v.x;
            l.a.y += v.y;
            l.a.z += v.z;
            l.b.x += v.x;
            l.b.y += v.y;
            l.b.z += v.z;
        }
    }

    void rotate_sphere(float alpha, float beta, float gamma) {
        // rotate all the lines in sphere_lines about (0, 0, 0)
        // alpha = rotation around X
        // beta  = rotation around Y
        // gamma = rotation around Z

        const float ca = cos(alpha);
        const float sa = sin(alpha);
        const float cb = cos(beta);
        const float sb = sin(beta);
        const float cg = cos(gamma);
        const float sg = sin(gamma);

        // R = Rz * Ry * Rx
        const float r00 = cg * cb;
        const float r01 = cg * sb * sa - sg * ca;
        const float r02 = cg * sb * ca + sg * sa;

        const float r10 = sg * cb;
        const float r11 = sg * sb * sa + cg * ca;
        const float r12 = sg * sb * ca - cg * sa;

        const float r20 = -sb;
        const float r21 = cb * sa;
        const float r22 = cb * ca;

        auto rotate = [&](demo::Vec3& p) {
            const float x = p.x;
            const float y = p.y;
            const float z = p.z;

            p.x = r00 * x + r01 * y + r02 * z;
            p.y = r10 * x + r11 * y + r12 * z;
            p.z = r20 * x + r21 * y + r22 * z;
        };

        for (Line3& l : sphere_lines) {
            rotate(l.a);
            rotate(l.b);
        }

        rotate(sphere_center);
    }

    void draw_line3(demo::Canvas& canvas, const Line3& line) {
        // add perspective later
        const float zfactor = 0;
        canvas.line({
                        line.a.x / (1 + line.a.z * zfactor),
                        line.a.y / (1 + line.a.z * zfactor)
                    }, {
                        line.b.x / (1 + line.b.z * zfactor), 
                        line.b.y / (1 + line.b.z * zfactor)
                    });
    }
    
    void draw_sphere(demo::Canvas& canvas, bool seethrough) {
        canvas.circle({sphere_center.x, sphere_center.y}, sphere_radius);
        for (Line3& line : sphere_lines) {
            if (seethrough || line.a.z >= 0 || line.b.z >= 0)
                draw_line3(canvas, line);
        }
    }

public:
    const char* title() const override { return "Earth demo"; };
    demo::Vec2 preferred_size() const override { return {600, 600}; }

    EarthSphereDemo() {
        create_sphere_lines(200.0, 6, 6, 60);
        rotate_sphere(0.6, 0, 0);
        translate_sphere({preferred_size().x / 2, preferred_size().y / 2, 0.0});
    }

    void update(float dt, const demo::InputData& input) override {
        translate_sphere({-preferred_size().x / 2, -preferred_size().y / 2, 0.0});
        rotate_sphere(-0.2 * dt, 1.0 * dt, 0.01 * dt);
        translate_sphere({preferred_size().x / 2, preferred_size().y / 2, 0.0});
    }

    void draw(demo::Canvas& canvas) override {
        canvas.fill({ 80, 80, 100, 255 });
        canvas.clear();
        canvas.stroke({ 80, 80, 100, 255 });
        canvas.fill({120, 120, 130, 255});
        draw_sphere(canvas, false);
    }
};

std::unique_ptr<demo::Demo> demo::create_demo() {
    return std::make_unique<EarthSphereDemo>();
}