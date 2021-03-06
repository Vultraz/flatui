// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "fplbase/renderer.h"
#include "fplbase/input.h"
#include "fplbase/utilities.h"
#include "flatui/flatui.h"
#include "flatui/flatui_common.h"
#include <cassert>

using flatui::Run;
using flatui::ImageButton;
using flatui::CheckBox;
using flatui::TextButton;
using flatui::SetMargin;
using flatui::ColorBackground;
using flatui::Edit;
using flatui::EndGroup;
using flatui::EndScroll;
using flatui::Image;
using flatui::ImageBackgroundNinePatch;
using flatui::Label;
using flatui::Margin;
using flatui::ModalGroup;
using flatui::SetVirtualResolution;
using flatui::SetTextColor;
using flatui::Slider;
using flatui::StartGroup;
using flatui::StartScroll;
using mathfu::vec2;
using mathfu::vec2i;
using mathfu::vec4;

extern "C" int FPL_main(int /*argc*/, char **argv) {
  fplbase::Renderer renderer;
  fplbase::InputSystem input;
  flatui::FontManager fontman;
  fplbase::AssetManager assetman(renderer);

  // Set the local directory to the assets for this test.
  bool result = fplbase::ChangeToUpstreamDir(argv[0], "test/assets");
  assert(result);

  // Initialize stuff.
  renderer.Initialize(vec2i(800, 600), "FlatUI test");
  renderer.SetCulling(fplbase::Renderer::kCullBack);
  input.Initialize();

  // Open OpenType font.
  fontman.Open("fonts/NotoSansCJKjp-Bold.otf");
  fontman.SetRenderer(renderer);

  // Load textures.
  auto tex_about = assetman.LoadTexture("textures/text_about.webp");
  auto tex_check_on = assetman.LoadTexture("textures/btn_check_on.webp");
  auto tex_check_off = assetman.LoadTexture("textures/btn_check_off.webp");
  auto tex_circle = assetman.LoadTexture("textures/white_circle.webp");
  auto tex_bar = assetman.LoadTexture("textures/gray_bar.webp");
  assetman.StartLoadingTextures();

  // Wait for everything to finish loading...
  while (assetman.TryFinalize() == false) {
    renderer.AdvanceFrame(input.minimized(), input.Time());
  }

  // Main loop.
  while (!input.exit_requested()) {
    input.AdvanceFrame(&renderer.window_size());
    renderer.AdvanceFrame(input.minimized(), input.Time());

    const float kColorGray = 0.25f;
    renderer.ClearFrameBuffer(
        vec4(kColorGray, kColorGray, kColorGray, 1.0f));

    // Draw GUI test
    static float f = 0.0f;
    f += 0.04f;
    static bool show_about = false;
    static vec2 scroll_offset(mathfu::kZeros2f);
    static bool checkbox1_checked;
    static float slider_value;
    static std::string str("Edit box.");
    static std::string str2("More Edit box.");
    static std::string str3("The\nquick brown fox jumps over the lazy dog.\n"
                            "The quick brown fox jumps over the lazy dog. "
                            "The quick brown fox jumps over the lazy dog. "
                            "The quick brown fox jumps over the lazy dog. ");

    auto click_about_example = [&](const char *id, bool about_on) {
      if (ImageButton(*tex_about, 50, Margin(10), id) ==
          flatui::kEventWentUp) {
        fplbase::LogInfo("You clicked: %s", id);
        show_about = about_on;
      }
    };

    Run(assetman, fontman, input, [&]() {
      SetVirtualResolution(1000);
      StartGroup(flatui::kLayoutOverlay, 0);
        StartGroup(flatui::kLayoutHorizontalTop, 10);
          PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                        mathfu::kZeros2f);
          StartGroup(flatui::kLayoutVerticalLeft, 20);
            click_about_example("my_id1", true);
            Edit(30, vec2(400, 30), "edit2", &str2);
            StartGroup(flatui::kLayoutHorizontalTop, 0);
              Edit(30, vec2(0, 30), "edit", &str);
              Label(">Tail", 30);
            EndGroup();
            Slider(*tex_circle, *tex_bar,
                   vec2(300, 25), 0.5f, "slider", &slider_value);
            CheckBox(*tex_check_on, *tex_check_off, "CheckBox", 30,
                          Margin(6, 0), &checkbox1_checked);
            StartGroup(flatui::kLayoutHorizontalTop, 0);
              Label("Property T", 30);
              SetTextColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
              Label("Test ", 30);
              SetTextColor(mathfu::kOnes4f);
              Label("ffWAWÄテスト", 30);
            EndGroup();
            if (TextButton("text button test", 20, Margin(10)) ==
                flatui::kEventWentUp) {
              fplbase::LogInfo("You clicked: text button");
            }
            StartGroup(flatui::kLayoutVerticalLeft, 20, "scroll");
            StartScroll(vec2(200, 100), &scroll_offset);
                ImageBackgroundNinePatch(*tex_about,
                                         vec4(0.2f, 0.2f, 0.8f, 0.8f));
                click_about_example("my_id4", true);
                Label("The quick brown fox jumps over the lazy dog", 24);
                Label("The quick brown fox jumps over the lazy dog", 20);
              EndScroll();
            EndGroup();
          EndGroup();
          StartGroup(flatui::kLayoutVerticalCenter, 40);
            click_about_example("my_id2", true);
            Image(*tex_about, 40);
            Image(*tex_about, 30);
          EndGroup();
          StartGroup(flatui::kLayoutVerticalRight, 0);
            Edit(24, vec2(400, 400), "edit3", &str3);
          EndGroup();
        EndGroup();
        if (show_about) {
          StartGroup(flatui::kLayoutVerticalLeft, 20, "about_overlay");
            ModalGroup();
            PositionGroup(flatui::kAlignRight,
                          flatui::kAlignTop, vec2(-30, 30));
            SetMargin(Margin(10));
            ColorBackground(vec4(0.5f, 0.5f, 0.0f, 1.0f));
            click_about_example("my_id3", false);
            Label("This is the about window! すし!", 32);
            Label("You should only be able to click on the", 24);
            Label("about button above, not anywhere else", 20);
          EndGroup();
        }
      EndGroup();
    });
  }

  return 0;
}
