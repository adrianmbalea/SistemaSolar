/*
*        PROYECTO SISTEMA SOLAR
*
*    Autores: Adrian MartÌnez Balea y Xabier Primoi Martinez
*
* Funcionamiento: Representacion del sistema solar con un menu (haciendo click derecho) que
* permite tener diferentes perspectivas desde diferentes cuerpos celestes. Ademas, permite
* girar las orbitas modificando su angulo con las teclas.
*
*
**/


//#define GL_SILENCE_DEPRECATION
#include <windows.h>
#include <glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Para texturas
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLuint textura;

#define PI 3.1416f
#define INCREMENTO .01 // Incremento del angulo en el giro de la camara voyager
#define MYTIEMPO 50

// tamanho de ventana
int W_WIDTH = 700;
int W_HEIGHT = 700;


float alpha = 0; // giro horizontal
float beta = 0;  // giro vertical
int camara = 1;  // numero de camara, se inicializa con voyager

int esfera;
int cubo;
int flag_orbitas = 1;   //si esta activada, se muestran las orbitas
int flag_ejes = 1;        //si esta activada, se muestran los ejes
int flag_sol = 1;

//Variables de iluminacion
GLfloat Ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat Diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat SpecRef[]  = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

//Variables de definicion de la posicion del foco y direccion de iluminacion
GLfloat LuzPos[]  = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat SpotDir[] = {1.0f, 1.0f, 1.0f};

// estructura de los cuerpos celestes
typedef struct {
    int id;
    int geometria;
    float distancia;
    float v_trans;
    float angulo_trans;
    float v_rot;
    float angulo_rot;
    float tamano;
    float red;
    float green;
    float blue;
    GLuint textura;
}objeto;

//                    id  geometria  distancia v_trans  angulo_trans    v_rot  angulo_rot  tamano     red    green   blue   textura
objeto        sol = { 0,      0,       0.00f,   0.00f,     0.00f,      10.00f,   0.00f,    1.000f,   1.00f,  1.00f,  0.00f,    0 };
objeto   mercurio = { 1,      0,       2.00f,   9.20f,     0.00f,      50.00f,   0.00f,    0.107f,   1.00f,  0.55f,  0.32f,    0 };
objeto      venus = { 2,      0,       3.00f,   6.73f,     0.00f,      30.00f,   0.00f,    0.354f,   1.00f,  0.62f,  0.00f,    0 };
objeto     tierra = { 3,      0,       4.50f,   5.71f,     0.00f,      10.00f,   0.00f,    0.378f,   0.00f,  0.00f,  1.00f,    0 };
objeto       luna = { 4,      0,       1.10f,   0.20f,     0.00f,      10.00f,   0.00f,    0.100f,   1.00f,  1.00f,  1.00f,    0 };
objeto        ISS = { 5,      1,       0.80f,   1.47f,     0.00f,      15.00f,   0.00f,    0.050f,   0.31f,  0.31f,  0.31f,    0 };
objeto      marte = { 6,      0,       6.00f,   4.63f,     0.00f,      10.00f,   0.00f,    0.196f,   0.75f,  0.16f,  0.00f,    0 };
objeto    jupiter = { 7,      0,       7.50f,   2.50f,     0.00f,      50.00f,   0.00f,    0.654f,   0.96f,  0.69f,  0.47f,    0 };
objeto    saturno = { 8,      0,       9.50f,   1.85f,     0.00f,      20.00f,   0.00f,    0.562f,   0.96f,  0.84f,  0.34f,    0 };
objeto      urano = { 9,      0,       11.00f,  1.31f,     0.00f,      40.00,    0.00f,    0.425f,   0.25f,  0.76f,  0.79f,    0 };
objeto    neptuno = { 10,     0,       12.50f,  1.04f,     0.00f,      10.0f,    0.00f,    0.400f,   0.06f,  0.49f,  0.65f,    0 };


// Funciones de dibujo
extern int myEsfera(void);
extern int myCuadrado(void);
void myOrbita(float radio);
void myEjes(objeto planeta);
void display_anillos();
void display_planeta(objeto planeta);
void Display();
GLuint cargaTexturas(const char *imagen);

// Funciones de movimiento
void movimiento_planeta(objeto* planeta);
void myMovimiento(int value);

// Funciones de camaras
void myCamara();
void myTelescopio(objeto planeta, objeto objeto);
void myTelescopioSatelite(objeto planeta, objeto objeto);

// Funciones de menu
void onMenu(int opcion);
void myMenu(void);

// Otras de opengl, configuraciones...
void myTeclasespeciales(int cursor, int x, int y);
double radianes(double grados);
void changeSize(GLint newWidth, GLint newHeight);

void OpenGlInit()
{
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);

    //Definimos las iluminaciones
    glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, LuzPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, SpotDir);

    //Efectos de foco
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 179.0f);

    //Activamos las luces
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    //Definimos el seguimiento del color como
    //propiedad luminosa de los materiales
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    //Definimos las propiedades de brillo metalico
    glMaterialfv(GL_FRONT, GL_SPECULAR, SpecRef);
    glMateriali(GL_FRONT, GL_SHININESS, 1);
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    //Posicion de la ventana
    glutInitWindowPosition(100, 100);
    //Tamano de la ventana
    glutInitWindowSize(W_WIDTH, W_HEIGHT);
    //Tipo de color y buffer
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    //Crear la ventana
    glutCreateWindow("Sistema solar");
    
    OpenGlInit();
   
    esfera = myEsfera();
    cubo = myCuadrado();

    sol.textura      = cargaTexturas("sol.jpeg");
    mercurio.textura = cargaTexturas("mercurio.jpeg");
    venus.textura    = cargaTexturas("venus_atmosfera.jpeg");
    tierra.textura   = cargaTexturas("tierra.jpeg");
    marte.textura    = cargaTexturas("marte.jpeg");
    jupiter.textura  = cargaTexturas("jupiter.jpeg");
    saturno.textura  = cargaTexturas("saturno.jpeg");
    urano.textura    = cargaTexturas("urano.jpeg");
    neptuno.textura  = cargaTexturas("neptuno.jpeg");
    luna.textura     = cargaTexturas("luna.jpeg");
    ISS.textura      = cargaTexturas("ISS.jpg");

    glutSpecialFunc(myTeclasespeciales);
    //Funcion de dibujo
    glutDisplayFunc(Display);

    //Empieza el bucle principal
    glutReshapeFunc(changeSize);
    myMovimiento(10);
    myMenu();
    myCamara();

    glutMainLoop();

    exit(EXIT_SUCCESS);
}


/* FUNCIONES DE MOVIMIENTO */
void movimiento_planeta(objeto* planeta) {
    // aumenta el angulo de translacion (con respecto al sol, o a la tierra en el caso de ISS o luna)
    planeta->angulo_trans += planeta->v_trans;
    if (planeta->angulo_trans > 360) planeta->angulo_trans -= 360; // se fuerza a valores entre 0 y 359

    // aumenta el angulo de rotacion (sobre si mismo)
    planeta->angulo_rot += planeta->v_rot;
    if (planeta->angulo_rot > 360) planeta->angulo_rot -= 360; // se fuerza a valores entre 0 y 359
}

void myMovimiento(int value) {
    // produce el movimiento de todos los cuerpos
    movimiento_planeta(&sol);
    movimiento_planeta(&mercurio);
    movimiento_planeta(&venus);
    movimiento_planeta(&tierra);
    movimiento_planeta(&luna);
    movimiento_planeta(&ISS);
    movimiento_planeta(&marte);
    movimiento_planeta(&jupiter);
    movimiento_planeta(&saturno);
    movimiento_planeta(&urano);
    movimiento_planeta(&neptuno);


    glutPostRedisplay();
    glutTimerFunc(MYTIEMPO, myMovimiento, value);
}

/* FUNCIONES DE MENU */
void myMenu(void) {
    int menu;

    // se crea el menu con sus opciones
    menu = glutCreateMenu(onMenu);
    glutAddMenuEntry("Voyayer", 1);
    glutAddMenuEntry("Sol", 2);
    glutAddMenuEntry("Mercurio", 3);
    glutAddMenuEntry("Venus", 4);
    glutAddMenuEntry("Marte", 5);
    glutAddMenuEntry("Jupiter", 6);
    glutAddMenuEntry("Saturno", 7);
    glutAddMenuEntry("Urano", 8);
    glutAddMenuEntry("Neptuno", 9);
    glutAddMenuEntry("Tierra (desde la Luna)", 10);
    glutAddMenuEntry("Tierra (desde la ISS)", 11);
    glutAddMenuEntry("Dibujar orbitas", 12);
    glutAddMenuEntry("Borrar orbitas", 13);
    glutAddMenuEntry("Dibujar ejes", 14);
    glutAddMenuEntry("Borrar ejes", 15);
    glutAddMenuEntry("Iluminar Sol", 16);
    glutAddMenuEntry("Apagar Sol", 17);


    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void onMenu(int opcion) {
    if (opcion <= 11) { // las primeras 11 opciones del menu estan destinadas a la camara
        camara = opcion;
    }
    else {
        switch (opcion) {
        case 12:
            flag_orbitas = 1;    // se dibujan las orbitas
            break;
        case 13:
            flag_orbitas = 0;    // se ocultan las orbitas
            break;
        case 14:
            flag_ejes = 1;        // se dibujan los ejes
            break;
        case 15:
            flag_ejes = 0;        // se ocultan los ejes
            break;
        case 16:
            flag_sol = 1;         //se enciende el sol
            break;
        case 17:
            flag_sol = 0;         //se apaga el sol
            break;
        }
    }

    glutPostRedisplay();
}


/* FUNCIONES DE CAMARA */
void myCamara() {
    float aspecto = (float)W_WIDTH / (float)W_HEIGHT;

    //ConfiguraciÛn de la matriz de proyeccion
    glMatrixMode(GL_PROJECTION);

    //La ponemos a uno
    glLoadIdentity();

    // (left, right, bottom, top, near, far)
    glOrtho(-14.0f * aspecto, 14.0f * aspecto, -14.0f, 14.0f, -14.0f, 14.0f);

    // (eye position, center, up position)
    gluLookAt(((float)sin(alpha) * cos(beta)), ((float)sin(beta)), (cos(alpha) * cos(beta)), 0, 0, 0, 0, cos(beta), 0);
}

void myTelescopio(objeto planeta, objeto objeto) {
    float aspecto = (float)W_WIDTH / (float)W_HEIGHT;

    //Configuracion de la matriz de proyeccion
    glMatrixMode(GL_PROJECTION);

    //La ponemos a uno
    glLoadIdentity();

    // (fovy, aspect, near, far)
    gluPerspective(45.0f, aspecto, 0.01f, 2000.0f);

    // (eye position, center, up position)
    gluLookAt(planeta.distancia * cos(radianes(planeta.angulo_trans)), 0.0f, -1 * planeta.distancia * sin(radianes(planeta.angulo_trans)), objeto.distancia * cos(radianes(objeto.angulo_trans)), 0.0f, -1 * objeto.distancia * sin(radianes(objeto.angulo_trans)), 0.0f, 1.0f, 0.0f);
}

void myTelescopioSatelite(objeto planeta, objeto objeto) {
    float aspecto = (float)W_WIDTH / (float)W_HEIGHT;

    //Configuracion de la matriz de proyeccion
    glMatrixMode(GL_PROJECTION);

    //La ponemos a uno
    glLoadIdentity();

    // (fovy, aspect, near, far)
    gluPerspective(45.0f, aspecto, 0.01f, 2000.0f);

    float x_dist_sat_sol = planeta.distancia * cos(planeta.angulo_trans * PI / 180) + objeto.distancia * cos((planeta.angulo_trans + objeto.angulo_trans) * PI / 180);
    float z_dist_sat_sol = planeta.distancia * sin(planeta.angulo_trans * PI / 180) + objeto.distancia * sin((planeta.angulo_trans + objeto.angulo_trans) * PI / 180);

    // (eye position, center, up position)
    gluLookAt(x_dist_sat_sol, 0.0f, -1 * z_dist_sat_sol, planeta.distancia * cos(planeta.angulo_trans * PI / 180), 0.0f, -1 * planeta.distancia * sin(planeta.angulo_trans * PI / 180), 0.0f, 1.0f, 0.0f);
}



/* FUNCIONES DE DIBUJO */
void myOrbita(float radio) {
    float x, z;
    int numero_segmentos = 100;
    //Dibuja circunferencia
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numero_segmentos; i++) {
        x = (float)cos(i * 2 * PI / numero_segmentos);
        z = (float)sin(i * 2 * PI / numero_segmentos);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(radio * x, 0.0, radio * z);
    }
    glEnd();
    glFlush();
}


void myEjes(objeto planeta) {

    // Eje X
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(2 * planeta.tamano, 0.0f, 0.0f);
    glEnd();


    // Eje Y
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 2 * planeta.tamano, 0.0f);
    glEnd();


    // Eje Z
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0, 2 * planeta.tamano);
    glEnd();

}

void display_anillos() { // dibuja los 7 anillos de saturno
    for (float tamano = 1.1; tamano < 1.8; tamano += 0.1f) {
        myOrbita(tamano * saturno.tamano);
    }
}

void display_planeta(objeto planeta) {
    glPushMatrix(); // meto la matriz en la pila

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    if (flag_orbitas) myOrbita(planeta.distancia); // dibujo las orbitas si esta activada la flag
    glEnable(GL_TEXTURE_2D);
    if(flag_sol){ glEnable(GL_LIGHT0); glEnable(GL_LIGHTING); }
    glRotatef(planeta.angulo_trans, 0.0f, 1.0f, 0.0f); // roto alrededor del eje y
    glTranslatef(planeta.distancia, 0.0f, 0.0f); // transladamos alrededor del eje x
    glPushMatrix();
    glRotatef(planeta.angulo_rot, 0, 1, 0); // roto sobre el eje y
    if (flag_ejes) myEjes(planeta); // dibujo los ejes si la flag esta activada
    glScalef(planeta.tamano, planeta.tamano, planeta.tamano); // escalo con el tamanho correspondiente
    //glColor3f(planeta.red, planeta.green, planeta.blue); // le doy color
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, planeta.textura);
    if(planeta.geometria == 0) glCallList(esfera); // llamo a la lista para dibujar
    else glCallList(cubo);
    glPopMatrix(); // desapilo de la pila


    if (planeta.id == tierra.id) { // si es la tierra, se dibuja ademas la luna y las iss con referencia a esta
        display_planeta(luna);
        display_planeta(ISS);
    }

    if (planeta.id == saturno.id) { // si es saturno, se dibujan sus anillos
        display_anillos();
    }

    glPopMatrix(); // vuelvo a desapilar para volver a tener como referencia el sol
}

// FunciÛn de display
void Display() {
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (camara) {
    case 1:
        myCamara();
        break;
    case 2:
        myTelescopio(tierra, sol);
        break;
    case 3:
        myTelescopio(tierra, mercurio);
        break;
    case 4:
        myTelescopio(tierra, venus);
        break;
    case 5:
        myTelescopio(tierra, marte);
        break;
    case 6:
        myTelescopio(tierra, jupiter);
        break;
    case 7:
        myTelescopio(tierra, saturno);
        break;
    case 8:
        myTelescopio(tierra, urano);
        break;
    case 9:
        myTelescopio(tierra, neptuno);
        break;
    case 10:
        myTelescopioSatelite(tierra, luna);
        break;
    case 11:
        myTelescopioSatelite(tierra, ISS);
        break;
    }

    // Se activa la matriz del modelador
    glMatrixMode(GL_MODELVIEW);

    // Inicializamos la matriz del modelo a la identidad
    glLoadIdentity();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    if (flag_sol) glDisable(GL_LIGHT0); glDisable(GL_LIGHTING);

    // Dibujar el sol
    glPushMatrix();
        glRotatef(sol.angulo_rot, 0, 1, 0);
        glPushMatrix();
            if (flag_ejes) myEjes(sol);
            glScalef(sol.tamano, sol.tamano, sol.tamano);
            glColor3f(sol.red, sol.green, sol.blue);
            glBindTexture(GL_TEXTURE_2D, sol.textura);
            glCallList(esfera);
        glPopMatrix();
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Dibujar planetas
    display_planeta(mercurio);
    display_planeta(venus);
    display_planeta(tierra);
    display_planeta(marte);
    display_planeta(jupiter);
    display_planeta(saturno);
    display_planeta(urano);
    display_planeta(neptuno);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    // Se limpian los buffers
    glFlush();
    glutSwapBuffers();
}


/* OTRAS FUNCIONES */
void changeSize(GLint newWidth, GLint newHeight) {
    W_WIDTH = newWidth;
    W_HEIGHT = newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)W_WIDTH / (float)W_HEIGHT, 0.01f, 500.0f);
}


double radianes(double grados) {
    return grados * PI / 180;
}


void myTeclasespeciales(int cursor, int x, int y)
{
    switch (cursor)
    {
        //Giros:
    case GLUT_KEY_UP:
        beta += INCREMENTO;
        break;
    case GLUT_KEY_DOWN:
        beta -= INCREMENTO;
        break;
    case GLUT_KEY_RIGHT:
        alpha -= INCREMENTO;
        break;
    case GLUT_KEY_LEFT:
        alpha += INCREMENTO;
        break;
    default:
        break;
    }

    if (alpha >= 2.0 * PI || alpha <= 0) alpha = 0;
    if (beta >= 2.0 * PI || beta <= 0) beta = 0;

    glutPostRedisplay();
}


GLuint cargaTexturas(const char *imagen)
{
    
    //Carga y crea la textura
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    
    unsigned char *data = stbi_load(imagen, &width, &height, &nrChannels, 0);
    if (data){
        
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    
    stbi_image_free(data);
    return textura;
}
