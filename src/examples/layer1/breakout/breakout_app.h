////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Triangle example: The most basic OpenGL application
//
// Level: 1
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//
//


namespace octet {

  class color_brick {

  public: 
    mat4t modelToWorld;

    // half the width of the box
    float halfWidth;

    // half the height of the box
    float halfHeight;

    // what color is our box
    vec4 color;

		bool _state;

		// These floats contain the world position of the box
		float worldX, worldY;

    color_brick() {
		  int bomb = 4;
    }

		void enabled(bool state) {
			_state = state;
		}

    void init(const vec4 &_color, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
			worldX = x;
			worldY = y;
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      color = _color;
			_state = true;
    }

    void render(color_shader &shader, mat4t &cameraToWorld) {
      // If the state is enabled, render it on screen.
			if(_state) {
				// build a projection matrix: model -> world -> camera -> projection
				// the projection space is the cube -1 <= x/w, y/w, z/w <= 1
				mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

				// set up the uniforms for the shader
				shader.render(modelToProjection, color);

				// this is an array of the positions of the corners of the box in 3D
				// a straight "float" here means this array is being generated here at runtime.
				float vertices[] = {
					-halfWidth, -halfHeight, 0,
					 halfWidth, -halfHeight, 0,
					 halfWidth,  halfHeight, 0,
					-halfWidth,  halfHeight, 0,
				};

				// attribute_pos (=0) is position of each corner
				// each corner has 3 floats (x, y, z)
				// there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
				glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
				glEnableVertexAttribArray(attribute_pos);
    
				// finally, draw the box (4 vertices)
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
    }

    // move the object
    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
			worldX += x;
			worldY += y;
    }

    // position the object relative to another.
    void set_relative(color_brick &rhs, float x, float y) {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
			worldX = rhs.worldX + x;
			worldY = rhs.worldY + y;
    }

    // return true if this box collides with another.
    // note the "const"s which say we do not modify either box
    bool collides_with(const color_brick &rhs) const {

		 if (rhs._state) {
			float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
			float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

			// both distances have to be under the sum of the halfwidths
			// for a collision
			return
			(fabsf(dx) < halfWidth + rhs.halfWidth) &&
			(fabsf(dy) < halfHeight + rhs.halfHeight)
			;
		 }
		 else return false;
		 

		//float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
		//float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

		//// both distances have to be under the sum of the halfwidths
		//// for a collision
		//return
		//	(fabsf(dx) < halfWidth + rhs.halfWidth) &&
		//	(fabsf(dy) < halfHeight + rhs.halfHeight)
		//;
		}
  };




  class texture_brick : public color_brick {

    // what texture is on our sprite
    int texture;
    
    int health;

  public:

    texture_brick() {
      health = 2;
      texture = 0;
      _state = true;
    }

    void init(int _texture, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      texture = _texture;
      _state = true;
    }

    void render(texture_shader &shader, mat4t &cameraToWorld) {
      // invisible sprite... used for gameplay.
      if (!texture) return;

      // is the block is disabled, dont render
      if(!_state) return;

      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      // use "old skool" rendering
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      shader.render(modelToProjection, 0);

      // this is an array of the positions of the corners of the sprite in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
    
      // this is an array of the positions of the corners of the texture in 2D
      static const float uvs[] = {
         0,  0,
         1,  0,
         1,  1,
         0,  1,
      };

      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);
    
      // finally, draw the sprite (4 vertices)
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    // move the object
    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
    }

    bool is_above(const texture_brick &rhs, float margin) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];

      return
        (fabsf(dx) < halfWidth + margin)
      ;
    }
  };



  class breakout_app : public octet::app {
    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t cameraToWorld;

    // shader to draw a solid color
    color_shader color_shader_;

    // shader to draw a textured triangle
    texture_shader texture_shader_;

    // what state is the game in?
    enum state_t {
      num_sound_sources = 8,
      state_serving,
      state_playing,
      state_game_over
    };
    state_t state;

    // counters for scores
    int score;
		int maxScore;

    // game objects

    // court components
    color_brick court[4];

    // bats
    color_brick bat[1];

		// blocks
		texture_brick block[24];

    // the ball
    texture_brick ball;

		// Sounds
		ALuint bounce, fail, blockBreak;
		unsigned cur_source;
		ALuint sources[num_sound_sources];

    ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }

    // velocity of the ball
    float ball_velocity_x;
    float ball_velocity_y;

    public:

    // move the objects before drawing
    void simulate() {

      if(is_key_down('M')) {
        printf("Sound Play\n");

        ALuint source = get_sound_source();
        alSourcei(source, AL_BUFFER, bounce);
        alSourcePlay(source);
      }

      // AD & <> To move the bat
      if (is_key_down('A') || is_key_down(key_left)) {
        bat[0].translate(-0.1f, 0);
      } else if (is_key_down('D') || is_key_down(key_right)) {
        bat[0].translate(+0.1f, 0);
      }

			// Clamp the bat to the scene
			if(bat[0].worldX > 4.3f) {
				float xDiff = bat[0].worldX - 4.3f;
				bat[0].translate(-xDiff, 0);
			}
			else if (bat[0].worldX < -4.3f) {
				float xDiff = bat[0].worldX - -4.3f;
				bat[0].translate(-xDiff, 0);
			}

      if (state == state_serving) {
        // if we are serving, glue the ball to the bat
        ball.set_relative(bat[0], 0, 0.3f);
        if (is_key_down(key_space)) {
          // space serves, changing the state
          state = state_playing;

					// Randomise if the ball is shot left or right
					int x = rand()%2;
					if(!x) x-=1;

          ball_velocity_x = 0.1f * x;
          ball_velocity_y = 0.1f;
        }
      } else if (state == state_playing) {
        // if we are playing, move the ball
        ball.translate(ball_velocity_x, ball_velocity_y);

        // check collision with the bats
        if (ball_velocity_y < 0 && ball.collides_with(bat[0])) {
          ball_velocity_y = -ball_velocity_y;
        }

        // check collision with the court top and bottom
        if (ball_velocity_y > 0 && ball.collides_with(court[1])) {
          ball_velocity_y = -ball_velocity_y;
          ALuint source = get_sound_source();
          alSourcei(source, AL_BUFFER, bounce);
          alSourcePlay(source);
        } else if (ball_velocity_y < 0 && ball.collides_with(court[0])) {
					state = state_serving;
          ALuint source = get_sound_source();
          alSourcei(source, AL_BUFFER, fail);
          alSourcePlay(source);
        }

				// Check Collision with blocks
				for(int i = 0; i < 24; i++) {
					if(ball.collides_with(block[i])) {
						// Create vectors for Ball and Block position
						vec2 blockPos = vec2(block[i].worldX, block[i].worldY);
						vec2 ballPos = vec2(ball.worldX, ball.worldY);

						// Calculate the difference to get a collision offset
						float xDiff = blockPos[0] - ballPos[0];
						float yDiff = blockPos[1] - ballPos[1];

						// if ball hits the top or bottom of the bat bounce in Y else X
						if(yDiff > 0.2f || yDiff < -0.2f) {
							ball_velocity_y = -ball_velocity_y;
						} else {
							ball_velocity_x = -ball_velocity_x;
						}
						
						block[i].enabled(false);
            ALuint source = get_sound_source();
            alSourcei(source, AL_BUFFER, blockBreak);
            alSourcePlay(source);
						score++;
						
						if(score == maxScore) {
							printf("Game Complete\n\n");
							state = state_serving;
						}

					}
				}

        // check collision with the court end zones
        if (ball.collides_with(court[2])) {
					ball_velocity_x = -ball_velocity_x;
          ALuint source = get_sound_source();
          alSourcei(source, AL_BUFFER, bounce);
          alSourcePlay(source);
        } else if (ball.collides_with(court[3])) {
					ball_velocity_x = -ball_velocity_x;
          ALuint source = get_sound_source();
          alSourcei(source, AL_BUFFER, bounce);
          alSourcePlay(source);
        }
      }
    }

  public:

    // this is called when we construct the class
    breakout_app(int argc, char **argv) : app(argc, argv) {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      color_shader_.init();
      texture_shader_.init();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);

      GLuint ballGraphic = resources::get_texture_handle(GL_RGBA, "assets/breakout/ball.gif");
      GLuint  batGraphic = resources::get_texture_handle(GL_RGBA, "assets/breakout/bouncer.gif");

      ball.init(ballGraphic, 0, 0, 0.25f, 0.25f);
      bat[0].init(vec4(0, 0, 1, 1), 0, -3.7, 1.0f, 0.2f);
      court[0].init(vec4(1, 1, 1, 1), 0, -4, 10, 0.1f);
      court[1].init(vec4(1, 1, 1, 1), 0,  4, 10, 0.1f);
      court[2].init(vec4(1, 1, 1, 1), -5, 0, 0.2f, 8);
      court[3].init(vec4(1, 1, 1, 1), 5,  0, 0.2f, 8);

			// Calculate the rows and columns
			const int rowCount = 6, columnCount = 3;
			maxScore = rowCount*columnCount;
			const int blocks = rowCount*columnCount;
			// The position of the first Block (Top Left)
			const float xPos = -4, yPos = 3.2f;

			const float rowSpace = ((xPos * -1) *2) / (rowCount -1);

			int blockCount = 0;

      GLuint brickGraphic = resources::get_texture_handle(GL_RGBA, "assets/breakout/brick.gif");

			// Create the grid
			for(int columns = 0; columns < columnCount; columns++)
			{
				for(int rows = 0; rows < rowCount; rows++) {
					block[blockCount].init(brickGraphic, xPos + (rowSpace * rows), yPos - (0.7f*columns), 1.0f, 0.5f);
					blockCount++;
				}
			}

      // sounds
      bounce = resources::get_sound_handle(AL_FORMAT_MONO16, "assets/breakout/bounce.wav");
      fail = resources::get_sound_handle(AL_FORMAT_MONO16, "assets/breakout/fail.wav");
      blockBreak = resources::get_sound_handle(AL_FORMAT_MONO16, "assets/breakout/break.wav");
      cur_source = 0;
      alGenSources(num_sound_sources, sources);

      state = state_serving;
      score = 0;
    }

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      simulate();

      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glEnable(GL_DEPTH_TEST);

      // draw the ball
      ball.render(texture_shader_, cameraToWorld);

			//draw the blocks
			for(int i = 0; i < 24; i++)
			{
				block[i].render(texture_shader_, cameraToWorld);
			}

      // draw the bat
      bat[0].render(color_shader_, cameraToWorld);

      // draw the court
      for (int i = 0; i != 4; ++i) {
        court[i].render(color_shader_, cameraToWorld);
      }

      // move the listener with the camera
      vec4 &cpos = cameraToWorld.w();
      alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());

    }
  };
}
