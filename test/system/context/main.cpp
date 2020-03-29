
// =============================================================================
// MIT License
//
// Copyright (c) 2017-2019 Fedorov Alexey
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// =============================================================================

#include <chrono>
#include <thread>

#include <common/utils.hpp>
#include <common/version.hpp>
#include <system/window.hpp>
#include <unit_test/suite.hpp>
#include <graphics/render.hpp>

class context_test : public framework::unit_test::Suite
{
public:
    context_test() : Suite("context_test")
    {
        add_test([this]() { main_loop(); }, "main_loop");
    }

private:
    void main_loop()
    {
        using namespace framework;
        using namespace framework::graphics;
        using namespace framework::system;

        Window::set_application_name("GL Test");

        Window main_window({640, 480}, "Context test");
        Render render(main_window.context());

        main_window.show();

        render.set_clear_color(0xFF00FFFF);

        const float32 max_total_time = 1000;
        float32 total_time           = 0;

        while (main_window.is_visible() && total_time < max_total_time) {
            main_window.process_events();

            render.display();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));

            total_time += 16;
        }
    }
};

int main()
{
    return run_tests(context_test());
}
