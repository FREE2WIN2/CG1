// This source code is property of the Computer Graphics and Visualization
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved

#include "Viewer.h"

#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/checkbox.h>

#include <gui/SliderHelper.h>

#include <iostream>

#include <OpenMesh/Core/IO/MeshIO.hh>

#include "glsl.h"

Viewer::Viewer()
        : AbstractViewer("CG1 Exercise 2") {
    SetupGUI();

    CreateShaders();
    CreateVertexBuffers();

    modelViewMatrix.setIdentity();
    projectionMatrix.setIdentity();

    camera().FocusOnBBox(nse::math::BoundingBox<float, 3>(Eigen::Vector3f(-1, -1, -1), Eigen::Vector3f(1, 1, 1)));
}

void Viewer::SetupGUI() {
    auto mainWindow = SetupMainWindow();

    //Create GUI elements for the various options
    chkHasDepthTesting = new nanogui::CheckBox(mainWindow, "Perform Depth Testing");
    chkHasDepthTesting->setChecked(true);

    chkHasFaceCulling = new nanogui::CheckBox(mainWindow, "Perform backface Culling");
    chkHasFaceCulling->setChecked(true);

    sldJuliaCX = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "JuliaC.X", std::make_pair(-1.0f, 1.0f),
                                                              0.45f, 2);
    sldJuliaCY = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "JuliaC.Y", std::make_pair(-1.0f, 1.0f),
                                                              -0.3f, 2);
    sldJuliaZoom = nse::gui::AddLabeledSliderWithDefaultDisplay(mainWindow, "Julia Zoom", std::make_pair(0.01f, 10.0f),
                                                                1.0f, 2);

    performLayout();
}

// Create and define the vertex array and add a number of vertex buffers
void Viewer::CreateVertexBuffers() {
    /*** Begin of task 2.2.3 ***
    Fill the positions-array and your color array with 12 rows, each
    containing 4 entries, to define a tetrahedron. */

    // Define 3 vertices for one face
    GLfloat positions[] = {
            -1, 1, 1, 1,
            1, -1, 1, 1,
            1, 1, -1, 1,

            -1, 1, 1, 1,
            1, 1, -1, 1,
            -1, -1, -1, 1,

            1, 1, -1, 1,
            1, -1, 1, 1,
            -1, -1, -1, 1,

            -1, 1, 1, 1,
            -1, -1, -1, 1,
            1, -1, 1, 1,
    };





    // Generate the vertex array
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
// Generate a position buffer to be appended to the vertex array
    glGenBuffers(1, &position_buffer_id);
    // Bind the buffer for subsequent settings
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
    // Supply the position data
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    // The buffer shall now be linked to the shader attribute
    // "in_position". First, get the location of this attribute in
    // the shader program
    GLuint vid = glGetAttribLocation(program_id, "in_position");

    // Enable this vertex attribute array
    glEnableVertexAttribArray(vid);
    // Set the format of the data to match the type of "in_position"
    glVertexAttribPointer(vid, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /*** Bonus Task 2 ***/
    /* Array with only x and y coord (w value is unnecessary because we don't draw this. So we don't need the w information)*/
    GLfloat xy_positions[] = {
            -1, 1,
            1, -1,
            1, 1,

            -1, 1,
            1, 1,
            -1, -1,

            1, 1,
            1, -1,
            -1, -1,

            -1, 1,
            -1, -1,
            1, -1
    };

    glGenBuffers(1, &position_xy_id);
    // Bind the buffer for subsequent settings
    glBindBuffer(GL_ARRAY_BUFFER, position_xy_id);
    // Supply the position data
    glBufferData(GL_ARRAY_BUFFER, sizeof(xy_positions), xy_positions, GL_STATIC_DRAW);
    GLuint pid = glGetAttribLocation(program_id, "in_position_xy");

    // Enable this vertex attribute array
    glEnableVertexAttribArray(pid);
    // Set the format of the data to match the type of "in_position"
    glVertexAttribPointer(pid, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /*** Begin of task 2.2.2 (a) ***
    Create another buffer that will store color information. This works nearly
    similar to the code above that creates the position buffer. Store the buffer
    id into the variable "color_buffer_id" and bind the color buffer to the
    shader variable "in_color". ***/

    /* 2.2.2 a
     *   GLfloat colors[] = {
     *      1.0, 0, 0, 1,
     *      0, 1.0, 0, 1,
     *      0, 0, 1.0, 1
     *   }
     */

    GLfloat colors[] = {
            0, 1.0, 0, 1,
            0, 1.0, 0, 1,
            0, 1.0, 0, 1,

            0, 0, 1.0, 1,
            0, 0, 1.0, 1,
            0, 0, 1.0, 1,

            1.0, 1.0, 0, 1,
            1.0, 1.0, 0, 1,
            1.0, 1.0, 0, 1,

            0, 1.0, 1.0, 1,
            0, 1.0, 1.0, 1,
            0, 1.0, 1.0, 1
    };

    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    GLuint cid = glGetAttribLocation(program_id, "in_color");
    glEnableVertexAttribArray(cid);
    glVertexAttribPointer(cid, 4, GL_FLOAT, GL_FALSE, 0, 0);
    /*** End of task 2.2.2 (a) ***/


    // Unbind the vertex array to leave OpenGL in a clean state
    glBindVertexArray(0);

}

//Checks if the given shader has been compiled successfully. Otherwise, prints an
//error message and throws an exception.
//  shaderId - the id of the shader object
//  name - a human readable name for the shader that is printed together with the error
void CheckShaderCompileStatus(GLuint shaderId, std::string name) {
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        std::cerr << "Error while compiling shader \"" << name << "\":" << std::endl;
        glGetShaderInfoLog(shaderId, 512, nullptr, buffer);
        std::cerr << "Error: " << std::endl << buffer << std::endl;
        throw std::runtime_error("Shader compilation failed!");
    }
}

// Read, Compile and link the shader codes to a shader program
void Viewer::CreateShaders() {
    std::string vs((char *) shader_vert, shader_vert_size);
    const char *vertex_content = vs.c_str();

    std::string fs((char *) shader_frag, shader_frag_size);
    const char *fragment_content = fs.c_str();

    /*** Begin of task 2.2.1 ***
    Use the appropriate OpenGL commands to create a shader object for
    the vertex shader, set the source code and let it compile. Store the
    ID of this shader object in the variable "vertex_shader_id". Repeat
    for the fragment shader. Store the ID in the variable "fragment_shader_id.
    Finally, create a shader program with its handle stored in "program_id",
    attach both shader objects and link them. For error checking, you can
    use the method "CheckShaderCompileStatus()" after the call to glCompileShader().
    */


    /* Create Shader */
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    /* set Source */
    glShaderSource(vertex_shader_id, 1, &vertex_content,
                   0); //void glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
    glShaderSource(fragment_shader_id, 1, &fragment_content, 0);
    /* Compile Shader */
    glCompileShader(vertex_shader_id);
    glCompileShader(fragment_shader_id);
    /* Check compilation */
    CheckShaderCompileStatus(vertex_shader_id, "Vertex Shader");
    CheckShaderCompileStatus(fragment_shader_id, "Fragment Shader");
    /* Create new program */
    program_id = glCreateProgram();
    /* Attach Shader */
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    /* Link Program */
    glLinkProgram(program_id);
    /* Use of Program not necessary (is used in drawContents)*/
    /* Detach after sucessful linking (optional)
    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id); */
    /*** End of task 2.2.1 ***/
}

void Viewer::drawContents() {
    Eigen::Vector2f juliaC(sldJuliaCX->value(), sldJuliaCY->value());
    float juliaZoom = sldJuliaZoom->value();

    //Get the transform matrices
    camera().ComputeCameraMatrices(modelViewMatrix, projectionMatrix);

    // If has_faceculling is set then enable backface culling
    // and disable it otherwise
    if (chkHasFaceCulling->checked())
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    // If has_depthtesting is set then enable depth testing
    // and disable it otherwise
    if (chkHasDepthTesting->checked())
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // Activate the shader program
    glUseProgram(program_id);

    /*** Begin of task 2.2.4 (b) ***
    Set the shader variables for the modelview and projection matrix.
    First, find the location of these variables using glGetUniformLocation and
    then set them with the command glUniformMatrix4fv.
    */

    GLint model_id = glGetUniformLocation(program_id,"modelview");
    GLint projection_id = glGetUniformLocation(program_id,"projection");

    glUniformMatrix4fv(model_id,1,GL_FALSE,modelViewMatrix.data());
    glUniformMatrix4fv(projection_id,1,GL_FALSE,projectionMatrix.data());



    // Bind the vertex array
    glBindVertexArray(vertex_array_id);
    // Draw the bound vertex array. Start at element 0 and draw 3 vertices

    glDrawArrays(GL_TRIANGLES, 0, 12);
    /*** End of task 2.2.4 (b) ***/

    /*** Task 2.2.6 Julia Fractal GUI ***/
    juliaC = Eigen::Vector2f(sldJuliaCX->value(),sldJuliaCY->value());
    juliaZoom = sldJuliaZoom->value();
    GLint zoom_id = glGetUniformLocation(program_id,"zoom");
    GLint seed_id = glGetUniformLocation(program_id,"seed");
    glUniform1f(zoom_id,juliaZoom);
    glUniform2f(seed_id,juliaC.x(),juliaC.y());



    // Unbind the vertex array
    glBindVertexArray(0);
    // Deactivate the shader program
    glUseProgram(0);
}