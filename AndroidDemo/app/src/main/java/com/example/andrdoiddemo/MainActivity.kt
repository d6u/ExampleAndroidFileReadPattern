package com.example.andrdoiddemo

import android.content.res.AssetManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import com.example.andrdoiddemo.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.sampleText.text = stringFromJNI()

        Log.d(MainActivity::class.java.simpleName, "dataDir is ${applicationContext.dataDir}")

        init(applicationContext.dataDir.toString(), applicationContext.assets)
    }

    /**
     * A native method that is implemented by the 'andrdoiddemo' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun init(dataDir: String, assetManager: AssetManager)

    companion object {
        // Used to load the 'andrdoiddemo' library on application startup.
        init {
            System.loadLibrary("andrdoiddemo")
        }
    }
}
