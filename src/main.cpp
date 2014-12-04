#include "ReliefApplication.h"
#include "ofAppGlutWindow.h"
#include "ofGLProgrammableRenderer.h"

int main() {
    
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(1024,768, OF_WINDOW);         // <-------- setup the GL context
    
    //ofAppGlutWindow window;
    //ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofRunApp(new ReliefApplication());
}
