//==============================================================================
#define MAKE_IN(port)  port##IN
#define MAKE_OUT(port) port##OUT
#define MAKE_DIR(port) port##DIR
#define MAKE_SEL(port) port##SEL
#define MAKE_IE(port)  port##IE
#define MAKE_IES(port) port##IES
#define MAKE_IFG(port) port##IFG
//==============================================================================
#define OUTA(port) MAKE_OUT(port)
#define IN(port)  MAKE_IN(port)
#define DIRA(port) MAKE_DIR(port)
#define SEL(port) MAKE_SEL(port)
#define IE(port)  MAKE_IE(port)
#define IES(port) MAKE_IES(port)
#define IFG(port) MAKE_IFG(port)