#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

int Quit=0;

static int B_WIDTH=1920;
static int B_HEIGHT=1080;

static SDL_bool shaders_supported;
static int      current_shader = 0;

enum {
    SHADER_COLOR,
    SHADER_TEXTURE,
    SHADER_TEXCOORDS,
    NUM_SHADERS
};

// const GLcharARB * FProgram=
// "uniform sampler2D video;"
// "float texel_width;"
// "float texture_width;"

// "void main()"
// "{"
// "  float red, green, blue;"
// "  vec4 luma_chroma;"
//   "float luma, chroma_u,  chroma_v;"
//   "float pixelx, pixely;"
//   "float xcoord, ycoord;"
//   "vec3 yuv;"
  
//   "// note: pixelx, pixely are 0.0 to 1.0 so next pixel horizontally"
//   "//  is not just pixelx + 1; rather pixelx + texel_width."

//   "pixelx = gl_TexCoord[0].x;"
//   "pixely = gl_TexCoord[0].y;"

//   "// if pixelx is even, then that pixel contains [Y U] and the "
//   "//    next one contains [Y V] -- and we want the V part."
//   "// if  pixelx is odd then that pixel contains [Y V] and the "
//   "//     previous one contains  [Y U] -- and we want the U part."
//   "// note: only valid for images whose width is an even number of"
//   "// pixels"
// 	"texture_width = 1920.0;"
// 	"texel_width = 1.;"
//   "xcoord = floor (pixelx * texture_width);"
//   "luma_chroma = texture2D(video, p);"
      
//   "// just look up the brightness"
//   "luma = (luma_chroma.r - 0.0625) * 1.1643;"
	  
//   "chroma_u = luma_chroma.g;"
//   "chroma_v = texture2D(video, vec2(pixelx + texel_width, pixely)).a; "
//  " chroma_u = chroma_u - 0.5;"
// "  chroma_v = chroma_v - 0.5;"
  
// "  red = luma + 1.5958 * chroma_v;"
// "  green = luma - 0.39173 * chroma_u - 0.81290 * chroma_v;"
// "  blue = luma + 2.017 * chroma_u;"

// "  // set the color based on the texture color"
// "  gl_FragColor = vec4(red, green, blue, 1.0);"
// "}";



const GLcharARB * FProgram=
  "uniform sampler2DRect Ytex;\n"
  "uniform sampler2DRect Utex,Vtex;\n"
  "void main(void) {\n"
  "  float nx,ny,r,g,b,y,u,v;\n"
  "  vec4 txl,ux,vx;"
  "  nx=gl_TexCoord[0].x;\n"
  "  ny=1080.0-gl_TexCoord[0].y;\n"
  "  y=texture2DRect(Ytex,vec2(nx,ny)).r;\n"
  "  u=texture2DRect(Utex,vec2(nx/2.0,ny/2.0)).r;\n"
  "  v=texture2DRect(Vtex,vec2(nx/2.0,ny/2.0)).r;\n"

  "  y=1.1643*(y-0.0625);\n"
  "  u=u-0.5;\n"
  "  v=v-0.5;\n"

  "  r=y+1.5958*v;\n"
  "  g=y-0.39173*u-0.81290*v;\n"
  "  b=y+2.017*u;\n"

  "  gl_FragColor=vec4(r,g,b,1.0);\n"
  "}\n";

    static PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
    static PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
    static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
    static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
    static PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
    static PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
    static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
    static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
    static PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
    static PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
    static PFNGLUNIFORM1IARBPROC glUniform1iARB;
    static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;


void InitGL(int Width, int Height)                    /* We call this right after our OpenGL window is created. */
{
    GLdouble aspect;

    glViewport(0, 0, Width, Height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);        /* This Will Clear The Background Color To Black */
    glClearDepth(1.0);                /* Enables Clearing Of The Depth Buffer */
    glDepthFunc(GL_LESS);                /* The Type Of Depth Test To Do */
    glEnable(GL_DEPTH_TEST);            /* Enables Depth Testing */
    glShadeModel(GL_SMOOTH);            /* Enables Smooth Color Shading */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                /* Reset The Projection Matrix */

    aspect = (GLdouble)Width / Height;
    glOrtho(-3.0, 3.0, -3.0 / aspect, 3.0 / aspect, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}

static SDL_bool InitShaders()
{
    int i;

    /* Check for shader support */
    shaders_supported = SDL_FALSE;
    if (SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
        SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
        SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
        SDL_GL_ExtensionSupported("GL_ARB_fragment_shader")) {
        glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
        glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
        glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
        glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
        glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
        glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
        glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
        glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
        glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
        glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
        glUniform1iARB = (PFNGLUNIFORM1IARBPROC) SDL_GL_GetProcAddress("glUniform1iARB");
        glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
        if (glAttachObjectARB &&
            glCompileShaderARB &&
            glCreateProgramObjectARB &&
            glCreateShaderObjectARB &&
            glDeleteObjectARB &&
            glGetInfoLogARB &&
            glGetObjectParameterivARB &&
            glGetUniformLocationARB &&
            glLinkProgramARB &&
            glShaderSourceARB &&
            glUniform1iARB &&
            glUseProgramObjectARB) {
            shaders_supported = SDL_TRUE;
        }
    }

    if (!shaders_supported) {
        return SDL_FALSE;
    }

    /* Compile all the shaders */
    // for (i = 0; i < NUM_SHADERS; ++i) {
    //     if (!CompileShaderProgram(&shaders[i])) {
    //         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to compile shader!\n");
    //         return SDL_FALSE;
    //     }
    // }

    /* We're done! */
    return SDL_TRUE;
}


class yuvSDL{
    public:
        void init();
        void refreshYUV(GLubyte *YYtex, GLubyte  *UUtex, GLubyte *VVtex);
        void refreshBRG(GLubyte *BRG);
        void destruct();

        //GLuint loadTexture();

    private:
        int done;
        SDL_Window *window;
        SDL_Surface *surface;
        SDL_Event evt;
        int i;
        GLhandleARB FSHandle,PHandle;
        char *s;
        FILE *fp;
        int Ysize = 1920*1080;
        int Usize = (1920/2) * (1080 / 2);
        int Vsize = (1920/2) * (1080 / 2);
};

void yuvSDL::init()
{

    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    // Initialize SDL for video output 
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Create a OpenGL screen 
    this->window = SDL_CreateWindow( "Shader Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, B_WIDTH, B_HEIGHT, SDL_WINDOW_OPENGL );
    if ( !window ) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    if ( !SDL_GL_CreateContext(window)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL context: %s\n", SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    
    // Loop, drawing and checking events 
    InitGL(B_WIDTH, B_HEIGHT);
    if (InitShaders()) {
        SDL_Log("Shaders supported, press SPACE to cycle them.\n");
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shaders not supported!\n");
    }
    this->done = 0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,B_WIDTH,0,B_HEIGHT,-1,1);
    glViewport(0,0,B_WIDTH,B_HEIGHT);
    glClearColor(0,0,0,0);
    glColor3f(1.0,0.84,0.0);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    

    /* Set up program objects. */
    PHandle=glCreateProgramObjectARB();
    FSHandle=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    
    /* Compile the shader. */
    glShaderSourceARB(FSHandle,1,&FProgram,NULL);
    
    glCompileShaderARB(FSHandle);

    /* Print the compilation log. */
    glGetObjectParameterivARB(FSHandle,GL_OBJECT_COMPILE_STATUS_ARB,&i);
    s = (char *) malloc(32768);
    glGetInfoLogARB(FSHandle,32768,NULL,s);
    printf("Compile Log: %s\n", s);
    free(s);

    

    /* Create a complete program object. */
    glAttachObjectARB(PHandle,FSHandle);
    glLinkProgramARB(PHandle);

    /* And print the link log. */
    s = (char *)malloc(32768);
    glGetInfoLogARB(PHandle,32768,NULL,s);
    printf("Link Log: %s\n", s);
    free(s);

    /* Finally, use the program. */
    glUseProgramObjectARB(PHandle);


    /* Load the textures. */
    GLubyte *Ytex,*Utex,*Vtex, *RGBtex;
    RGBtex=(GLubyte *) malloc(Ysize*3);
    Ytex=(GLubyte *) malloc(Ysize);
    Utex=(GLubyte *) malloc(Usize);
    Vtex=(GLubyte *) malloc(Vsize);

    

    /* This might not be required, but should not hurt. */
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,4);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,B_WIDTH,B_HEIGHT,0,GL_BGR,GL_UNSIGNED_BYTE,RGBtex);

    /* Select texture unit 1 as the active unit and bind the U texture. */
    glActiveTexture(GL_TEXTURE1);
    i=glGetUniformLocationARB(PHandle,"Utex");
    glUniform1iARB(i,1);  /* Bind Utex to texture unit 1 */
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,1);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV,0,GL_LUMINANCE, B_WIDTH / 2, B_HEIGHT / 2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,Utex);

    /* Select texture unit 2 as the active unit and bind the V texture. */
    glActiveTexture(GL_TEXTURE2);
    i=glGetUniformLocationARB(PHandle,"Vtex");
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,2);
    glUniform1iARB(i,2);  /* Bind Vtext to texture unit 2 */

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV,0,GL_LUMINANCE,B_WIDTH / 2,B_HEIGHT / 2,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,Vtex);

    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE0);
    i=glGetUniformLocationARB(PHandle,"Ytex");
    glUniform1iARB(i,0);  /* Bind Ytex to texture unit 0 */
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,3);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV,0,GL_LUMINANCE,B_WIDTH,B_HEIGHT,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,Ytex);

    free(Ytex);
    free(Utex);
    free(Vtex);
    free(RGBtex);
}


void yuvSDL::refreshYUV(GLubyte *YYtex, GLubyte  *UUtex, GLubyte *VVtex)
{
    glEnable(GL_TEXTURE_2D);

    /* Select texture unit 1 as the active unit and bind the U texture. */
    glActiveTexture(GL_TEXTURE1);
    i=glGetUniformLocationARB(PHandle,"Utex");
    glUniform1iARB(i,1);  /* Bind Utex to texture unit 1 */
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,1);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0, B_WIDTH / 2, B_HEIGHT / 2,GL_LUMINANCE,GL_UNSIGNED_BYTE,UUtex);

    /* Select texture unit 2 as the active unit and bind the V texture. */
    glActiveTexture(GL_TEXTURE2);
    i=glGetUniformLocationARB(PHandle,"Vtex");
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,2);
    glUniform1iARB(i,2);  /* Bind Vtext to texture unit 2 */

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0, B_WIDTH / 2,B_HEIGHT / 2,GL_LUMINANCE,GL_UNSIGNED_BYTE,VVtex);

    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE0);
    i=glGetUniformLocationARB(PHandle,"Ytex");
    glUniform1iARB(i,0);  /* Bind Ytex to texture unit 0 */
    glBindTexture(GL_TEXTURE_RECTANGLE_NV,3);
    
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,B_WIDTH,B_HEIGHT,GL_LUMINANCE,GL_UNSIGNED_BYTE,YYtex);

    if(SDL_PollEvent(&evt)) {
    switch(evt.type) {
    case  SDL_KEYDOWN:
    case  SDL_QUIT:
        Quit=1;
    break;
    }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

    glBegin(GL_QUADS);
    glTexCoord2i(0,0);
    glVertex2i(0,0);
    glTexCoord2i(B_WIDTH,0);
    glVertex2i(B_WIDTH,0);
    glTexCoord2i(B_WIDTH,B_HEIGHT);
    glVertex2i(B_WIDTH,B_HEIGHT);
    glTexCoord2i(0,B_HEIGHT);
    glVertex2i(0,B_HEIGHT);
    glEnd();

    /* Flip buffers. */

    glFlush();
    SDL_GL_SwapWindow(window);
} 

void yuvSDL::refreshBRG(GLubyte *BRG){
    glUseProgramObjectARB(0);
    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,B_WIDTH,B_HEIGHT,GL_BGR,GL_UNSIGNED_BYTE,BRG);

    if(SDL_PollEvent(&evt)) {
    switch(evt.type) {
    case  SDL_KEYDOWN:
    case  SDL_QUIT:
        Quit=1;
    break;
    }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

    glBegin(GL_QUADS);
    glTexCoord2i(0,0);
    glVertex2i(0,0);
    glTexCoord2i(500,0);
    glVertex2i(500,0);
    glTexCoord2i(500,500);
    glVertex2i(500,500);
    glTexCoord2i(0,500);
    glVertex2i(0,500);
    glEnd();

    /* Flip buffers. */

    glFlush();
    SDL_GL_SwapWindow(window);

}

void yuvSDL::destruct()
{
    /* Clean up before exit. */

    glUseProgramObjectARB(0);
    glDeleteObjectARB(FSHandle);

    SDL_Quit();
}