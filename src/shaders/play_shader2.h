////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Single texture shader with no lighting

namespace octet {
  class play_shader2 : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for texture sampler
    GLuint samplerIndex_;

    GLuint resolutionXIndex_;
    GLuint resolutionYIndex_;
  public:
    void init() {
      // this is the vertex shader.
      // it is called for each corner of each triangle
      // it inputs pos and uv from each corner
      // it outputs gl_Position and uv_ to the rasterizer
      const char vertex_shader[] = SHADER_STR(
        varying vec2 uv_;
        varying vec4 color_;

        attribute vec4 pos;
        attribute vec2 uv;
        attribute vec4 color;

        uniform mat4 modelToProjection;

        void main() { gl_Position = pos; uv_ = uv; color_ = color;}
      );

      // this is the fragment shader
      // after the rasterizer breaks the triangle into fragments
      // this is called for every fragment
      // it outputs gl_FragColor, the color of the pixel and inputs uv_
      const char fragment_shader[] = SHADER_STR(
        varying vec2 uv_;
        varying vec4 color_;

        uniform sampler2D sampler;
        uniform int resolutionX;
        uniform int resolutionY;

        const float radius = 0.5;

        void Disc()
        {
          vec2 resolution = vec2(float(resolutionX), float(resolutionY));
          vec2 pixelPosition = gl_FragCoord.xy / resolution.xy;

          float distance = distance(vec2(0.5, 0.5), pixelPosition);

          if(distance > radius)
          {
            discard;
          }
        }

        void main() { 
          //gl_FragColor = texture2D(sampler, uv_); 
          Disc();
          gl_FragColor = color_; 
        }
      );
    
      // use the common shader code to compile and link the shaders
      // the result is a shader program
      shader::init(vertex_shader, fragment_shader);

      // extract the indices of the uniforms to use later
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      samplerIndex_ = glGetUniformLocation(program(), "sampler");
      resolutionXIndex_ = glGetUniformLocation(program(), "resolutionX");
      resolutionYIndex_ = glGetUniformLocation(program(), "resolutionY");

    }

    void render(const mat4t &modelToProjection, int sampler, int resX, int resY) {
      // tell openGL to use the program
      shader::render();

      // customize the program with uniforms
      glUniform1i(samplerIndex_, sampler);
      glUniform1i(resolutionXIndex_, resX);
      glUniform1i(resolutionYIndex_, resY);
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
    }
  };
}
