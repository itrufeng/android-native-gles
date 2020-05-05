package com.example.nativegl

import android.content.Context
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MainActivity : AppCompatActivity() {

    private lateinit var glView: MyGLSurfaceView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glView = MyGLSurfaceView(this)
        setContentView(glView)
    }

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}

class MyGLSurfaceView(context: Context) : GLSurfaceView(context) {
    init {
        setEGLContextClientVersion(2)
        setRenderer(MyGLRender(context))
    }
}

class MyGLRender(val context: Context) : GLSurfaceView.Renderer {
    override fun onDrawFrame(gl: GL10?) {
        on_draw_frame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        on_surface_changed(width, height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        init_context(context.assets)
        on_surface_created()
    }

    external fun init_context(assetManager: AssetManager)
    external fun on_draw_frame()
    external fun on_surface_changed(width: Int, height: Int)
    external fun on_surface_created()
}