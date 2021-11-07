/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "gui.h"
#include "DroidSans.h"
#include "renderer.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

void cGui::onMousePosition(const Vectorf& pos)
{
    auto& io = ImGui::GetIO();
    // if (glfwGetWindowAttrib(m_window, GLFW_FOCUSED))
    io.MousePos = ImVec2(pos.x, pos.y);
    // io.MousePos = ImVec2(-1, -1);
}

void cGui::onMouseButton(int button, int action)
{
    if (button >= 0 && button < 3)
    {
        auto& io = ImGui::GetIO();
        io.MouseDown[button] = action != GLFW_RELEASE;
    }
}

void cGui::onScroll(const Vectorf& pos)
{
    auto& io = ImGui::GetIO();
    io.MouseWheel += pos.y;
}

void cGui::onKey(int key, int, int action)
{
    auto& io = ImGui::GetIO();

    io.KeysDown[key] = action != GLFW_RELEASE;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void cGui::onChar(uint32_t c)
{
    auto& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
    {
        io.AddInputCharacter((uint8_t)c);
    }
}

bool cGui::createDeviceObjects()
{
    auto& io = ImGui::GetIO();

    const ImWchar range[] = { 0x0020, 0xFFFF, 0 };
    io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, 16, nullptr, range);

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    m_fontTexture = cRenderer::createTexture();
    cRenderer::bindTexture(m_fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->SetTexID((void*)(uintptr_t)m_fontTexture);

    return true;
}

void cGui::invalidateDeviceObjects()
{
    if (m_fontTexture)
    {
        auto& io = ImGui::GetIO();
        io.Fonts->SetTexID(0);

        cRenderer::deleteTexture(m_fontTexture);
        m_fontTexture = 0;
    }
}

void cGui::init(GLFWwindow* window)
{
    m_window = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    // io.RenderDrawListsFn = nullptr;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.SetClipboardTextFn = [](void* user_data, const char* text) {
        glfwSetClipboardString((GLFWwindow*)user_data, text);
    };

    io.GetClipboardTextFn = [](void* user_data) {
        return glfwGetClipboardString((GLFWwindow*)user_data);
    };

    io.ClipboardUserData = m_window;

    auto& s = ImGui::GetStyle();
    s.WindowTitleAlign = { 0.5f, 0.5f };
    s.WindowRounding = 5.0f;
    s.WindowPadding = { 3.0f, 3.0f };
}

void cGui::shutdown()
{
    invalidateDeviceObjects();
    // ImGui::Shutdown();
    ImGui::DestroyContext();
}

void cGui::beginFrame()
{
    if (!m_fontTexture)
    {
        createDeviceObjects();
    }

    auto& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(m_window, &w, &h);
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime = m_time > 0.0 ? (float)(current_time - m_time) : (float)(1.0f / 60.0f);
    m_time = current_time;

    // Hide OS mouse cursor if ImGui is drawing it
    // glfwSetInputMode(m_window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    ImGui::NewFrame();
}

void cGui::endFrame()
{
    ImGui::Render();

    // This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
    // If text or lines are blurry when integrating ImGui in your engine:
    // - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
    auto draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    auto& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
    {
        return;
    }

    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);

    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + offsetof(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + offsetof(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (const GLvoid*)((const char*)vtx_buffer + offsetof(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                cRenderer::bindTexture((GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }

    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
