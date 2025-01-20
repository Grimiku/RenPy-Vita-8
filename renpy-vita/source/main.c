#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <Python.h>
#include <SDL2/SDL.h>
#include <psp2/sysmodule.h>
#include <psp2/power.h>
#include <psp2/appmgr.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/vshbridge.h> 
#include <gpu_es4/psp2_pvr_hint.h>

#define MAX_PATH 256

int _newlib_heap_size_user = 180 * 1024 * 1024;
unsigned int sceLibcHeapSize = 10 * 1024 * 1024;

PyMODINIT_FUNC PyInit_pygame_sdl2_color(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_controller(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_display(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_draw(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_error(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_event(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_gfxdraw(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_image(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_joystick(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_key(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_locals(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_mouse(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_power(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_pygame_time(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_rect(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_rwobject(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_render(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_scrap(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_surface(void);
PyMODINIT_FUNC PyInit_pygame_sdl2_transform(void);

PyMODINIT_FUNC PyInit__renpy(void);
PyMODINIT_FUNC PyInit_renpy_audio_filter(void);
PyMODINIT_FUNC PyInit_renpy_audio_renpysound(void);
PyMODINIT_FUNC PyInit_renpy_display_accelerator(void);
PyMODINIT_FUNC PyInit_renpy_display_quaternion(void);
PyMODINIT_FUNC PyInit_renpy_display_matrix(void);
PyMODINIT_FUNC PyInit_renpy_display_render(void);
PyMODINIT_FUNC PyInit_renpy_encryption(void);

PyMODINIT_FUNC PyInit_renpy_gl_gldraw(void);
PyMODINIT_FUNC PyInit_renpy_gl_glenviron_shader(void);
PyMODINIT_FUNC PyInit_renpy_gl_glrtt_copy(void);
PyMODINIT_FUNC PyInit_renpy_gl_glrtt_fbo(void);
PyMODINIT_FUNC PyInit_renpy_gl_gltexture(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2draw(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2mesh(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2mesh2(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2mesh3(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2model(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2polygon(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2shader(void);
PyMODINIT_FUNC PyInit_renpy_gl2_gl2texture(void);

PyMODINIT_FUNC PyInit_renpy_lexersupport(void);
PyMODINIT_FUNC PyInit_renpy_pydict(void);
PyMODINIT_FUNC PyInit_renpy_style(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_activate_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_styleclass(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_hover_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_idle_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_insensitive_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_selected_activate_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_selected_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_selected_hover_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_selected_idle_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_style_selected_insensitive_functions(void);
PyMODINIT_FUNC PyInit_renpy_styledata_stylesets(void);
PyMODINIT_FUNC PyInit_renpy_renpyvita(void);

PyMODINIT_FUNC PyInit_renpy_text_ftfont(void);
PyMODINIT_FUNC PyInit_renpy_text_hbfont(void);
PyMODINIT_FUNC PyInit_renpy_text_textsupport(void);
PyMODINIT_FUNC PyInit_renpy_text_texwrap(void);
PyMODINIT_FUNC PyInit_renpy_uguu_gl(void);
PyMODINIT_FUNC PyInit_renpy_uguu_uguu(void);

PyMODINIT_FUNC PyInit__renpybidi(void);

char app_dir_path[0x100];
char res_1080_path[0x100];
char res_720_path[0x100];
char app_gles_module_path[0x100];
char app_program_path[0x100];
char python_script_buffer[0x400];
char title_id[0xA];
wchar_t program[0x100];
wchar_t python_home[0x100];

SceUID pid = -1;

void show_error_and_exit(const char* message)
{
    Py_Finalize();
    printf("%s", message);
    Py_Exit(1);
}

int main(int argc, char* argv[])
{
    PVRSRV_PSP2_APPHINT hint;
    SceUID fd = -1;
    SceBool override = SCE_FALSE;
    char target_path[MAX_PATH];

    Py_NoSiteFlag = 1;
    Py_OptimizeFlag = 2;

    /* Ren'Py is a bit CPU heavy. Increase CPU clocks */
    scePowerSetArmClockFrequency(444);
    /* Ren'Py requires full RW access */
    sceAppMgrUmount("app0:");

    /* Because locations can be all over the place, get TITLEID to find the application directory */
    pid = sceKernelGetProcessId();
    sceAppMgrAppParamGetString(pid, 12, title_id, sizeof(title_id));

    /* Generate full path to application directory and set relative paths */
    snprintf(app_dir_path, sizeof(app_dir_path), "ux0:app/%s", title_id);
    snprintf(res_1080_path, sizeof(res_1080_path), "%s/1080", app_dir_path);
    snprintf(res_720_path, sizeof(res_720_path), "%s/720", app_dir_path);
    snprintf(app_gles_module_path, sizeof(app_gles_module_path), "%s/module", app_dir_path);
    snprintf(app_program_path, sizeof(app_program_path), "%s/eboot.bin", app_dir_path);

    /* We might need the full juice */
    scePowerSetGpuClockFrequency(222);

    /* Check Custom Resolutions */
    /* Alllow 1080i and 720p support on both PSTV and Vita if wanted (Needs Sharpscale) */
    if (fd = sceIoOpen(res_1080_path, SCE_O_RDONLY, 0777) > 0) {
        sceIoClose(fd);
        override = SCE_TRUE;
        /* This actually needs it for both systems,
         * so just set it here for the vita */
        SDL_setenv("VITA_RESOLUTION", "1080", 1);
    }
    else if (fd = sceIoOpen(res_720_path, SCE_O_RDONLY, 0777) > 0) {
        sceIoClose(fd);
        override = SCE_TRUE;
        /* Vita doesn't really need it, but could help */
        SDL_setenv("VITA_RESOLUTION", "720", 1);
    }
    /* At least Set 720p for PSTV */
    if (vshSblAimgrIsDolce()) {
        if (!override)
            SDL_setenv("VITA_RESOLUTION", "720", 1);
    }

    /* SDL env vars and hints */
    SDL_setenv("VITA_PVR_SKIP_INIT", "0", 1);

    /*We don't need to disable back-touchpad here as this is implemented by
    SDL_HINT_VITA_TOUCH_MOUSE_DEVICE default hint setting. At least as of time of writing this.*/

    /* Load Modules */
    sceKernelLoadStartModule("vs0:sys/external/libfios2.suprx", 0, NULL, 0, NULL, NULL);
    sceKernelLoadStartModule("vs0:sys/external/libc.suprx", 0, NULL, 0, NULL, NULL);
    snprintf(target_path, MAX_PATH, "%s/%s", app_gles_module_path, "libgpu_es4_ext.suprx");
    sceKernelLoadStartModule(target_path, 0, NULL, 0, NULL, NULL);
    snprintf(target_path, MAX_PATH, "%s/%s", app_gles_module_path, "libIMGEGL.suprx");
    sceKernelLoadStartModule(target_path, 0, NULL, 0, NULL, NULL);

    /* Set PVR Hints */
    PVRSRVInitializeAppHint(&hint);
    snprintf(hint.szGLES2, MAX_PATH, "%s/%s", app_gles_module_path, "libGLESv2.suprx");
    snprintf(hint.szWindowSystem, MAX_PATH, "%s/%s", app_gles_module_path, "libpvrPSP2_WSEGL.suprx");
    hint.ui32SwTexOpCleanupDelay = 16000; // Set to 16 milliseconds to prevent a pool of unfreed memory
    PVRSRVCreateVirtualAppHint(&hint);


    wchar_t *program = Py_DecodeLocale(app_program_path, NULL);
    Py_SetProgramName(program);

    static struct _inittab builtins[] = {

        {"pygame_sdl2.color", PyInit_pygame_sdl2_color},
        {"pygame_sdl2.controller", PyInit_pygame_sdl2_controller},
        {"pygame_sdl2.display", PyInit_pygame_sdl2_display},
        {"pygame_sdl2.draw", PyInit_pygame_sdl2_draw},
        {"pygame_sdl2.error", PyInit_pygame_sdl2_error},
        {"pygame_sdl2.event", PyInit_pygame_sdl2_event},
        {"pygame_sdl2.gfxdraw", PyInit_pygame_sdl2_gfxdraw},
        {"pygame_sdl2.image", PyInit_pygame_sdl2_image},
        {"pygame_sdl2.joystick", PyInit_pygame_sdl2_joystick},
        {"pygame_sdl2.key", PyInit_pygame_sdl2_key},
        {"pygame_sdl2.locals", PyInit_pygame_sdl2_locals},
        {"pygame_sdl2.mouse", PyInit_pygame_sdl2_mouse},
        {"pygame_sdl2.power", PyInit_pygame_sdl2_power},
        {"pygame_sdl2.pygame_time", PyInit_pygame_sdl2_pygame_time},
        {"pygame_sdl2.rect", PyInit_pygame_sdl2_rect},
        {"pygame_sdl2.render", PyInit_pygame_sdl2_render},
        {"pygame_sdl2.rwobject", PyInit_pygame_sdl2_rwobject},
        {"pygame_sdl2.scrap", PyInit_pygame_sdl2_scrap},
        {"pygame_sdl2.surface", PyInit_pygame_sdl2_surface},
        {"pygame_sdl2.transform", PyInit_pygame_sdl2_transform},        
       
        {"_renpy", PyInit__renpy},
        {"renpy.audio.filter", PyInit_renpy_audio_filter},
        {"renpy.audio.renpysound", PyInit_renpy_audio_renpysound},
        {"renpy.display.accelerator", PyInit_renpy_display_accelerator},
        {"renpy.display.matrix", PyInit_renpy_display_matrix},
        {"renpy.display.quaternion", PyInit_renpy_display_quaternion},
        {"renpy.display.render", PyInit_renpy_display_render},
        {"renpy.encryption", PyInit_renpy_encryption},

        {"renpy.gl.gldraw", PyInit_renpy_gl_gldraw},
        {"renpy.gl.glenviron_shader", PyInit_renpy_gl_glenviron_shader},
        {"renpy.gl.glrtt_copy", PyInit_renpy_gl_glrtt_copy},
        {"renpy.gl.glrtt_fbo", PyInit_renpy_gl_glrtt_fbo},
        {"renpy.gl.gltexture", PyInit_renpy_gl_gltexture},
        {"renpy.gl2.gl2draw", PyInit_renpy_gl2_gl2draw},
        {"renpy.gl2.gl2mesh", PyInit_renpy_gl2_gl2mesh},
        {"renpy.gl2.gl2mesh2", PyInit_renpy_gl2_gl2mesh2},
        {"renpy.gl2.gl2mesh3", PyInit_renpy_gl2_gl2mesh3},
        {"renpy.gl2.gl2model", PyInit_renpy_gl2_gl2model},
        {"renpy.gl2.gl2polygon", PyInit_renpy_gl2_gl2polygon},
        {"renpy.gl2.gl2shader", PyInit_renpy_gl2_gl2shader},
        {"renpy.gl2.gl2texture", PyInit_renpy_gl2_gl2texture},

        {"renpy.lexersupport", PyInit_renpy_lexersupport},
        {"renpy.pydict", PyInit_renpy_pydict},
        {"renpy.style", PyInit_renpy_style},
        {"renpy.styledata.style_activate_functions", PyInit_renpy_styledata_style_activate_functions},
        {"renpy.styledata.styleclass", PyInit_renpy_styledata_styleclass},
        {"renpy.styledata.style_functions", PyInit_renpy_styledata_style_functions},
        {"renpy.styledata.style_hover_functions", PyInit_renpy_styledata_style_hover_functions},
        {"renpy.styledata.style_idle_functions", PyInit_renpy_styledata_style_idle_functions},
        {"renpy.styledata.style_insensitive_functions", PyInit_renpy_styledata_style_insensitive_functions},
        {"renpy.styledata.style_selected_activate_functions", PyInit_renpy_styledata_style_selected_activate_functions},
        {"renpy.styledata.style_selected_functions", PyInit_renpy_styledata_style_selected_functions},
        {"renpy.styledata.style_selected_hover_functions", PyInit_renpy_styledata_style_selected_hover_functions},
        {"renpy.styledata.style_selected_idle_functions", PyInit_renpy_styledata_style_selected_idle_functions},
        {"renpy.styledata.style_selected_insensitive_functions", PyInit_renpy_styledata_style_selected_insensitive_functions},
        {"renpy.styledata.stylesets", PyInit_renpy_styledata_stylesets},
        {"renpy.text.ftfont", PyInit_renpy_text_ftfont},
        {"renpy.text.hbfont", PyInit_renpy_text_hbfont},
        {"renpy.text.textsupport", PyInit_renpy_text_textsupport},
        {"renpy.text.texwrap", PyInit_renpy_text_texwrap},
        {"renpy.uguu.gl", PyInit_renpy_uguu_gl},
        {"renpy.uguu.uguu", PyInit_renpy_uguu_uguu},
        {"renpy.renpyvita", PyInit_renpy_renpyvita},

        {"_renpybidi", PyInit__renpybidi},

        {NULL, NULL}
    };

    PyImport_ExtendInittab(builtins);

    wchar_t *python_home = Py_DecodeLocale(app_dir_path, NULL);
    Py_SetPythonHome(python_home);

    Py_Initialize();

    wchar_t* pyargs[] = { python_home, NULL };
    PySys_SetArgvEx(1, pyargs, 1);
    
    snprintf(python_script_buffer, sizeof(python_script_buffer), "%s/renpy.py", app_dir_path);

    FILE* renpy_file = fopen((const char*)python_script_buffer, "rb");
    if (renpy_file == NULL)
    {
        show_error_and_exit("Could not open renpy.py after Python initialization.\n\nThis is an internal error and should not occur during normal usage.");
    }
    else
    {
        /* This is where the fun begins */
        PyRun_SimpleFile(renpy_file, "renpy.py");
    }

    Py_Finalize();
    return 0;
}
