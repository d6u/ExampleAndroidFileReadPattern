package com.example.assetpack

import android.content.Intent
import android.content.res.AssetManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.assetpack.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.text = stringFromJNI()

        val dataDir: String = applicationContext.dataDir.toString()

        Log.d(TAG, "dataDir is $dataDir")

        when (Action.fromIntent(intent)) {
            Action.INIT -> {
                init(applicationContext.assets, dataDir)
            }

            Action.ASSET_READ_ONE_GO -> {
                assetReadOneGo(applicationContext.assets)
            }

            Action.ASSET_READ_MULTIPLE_GO -> {
                val pieces = intent.extras?.getInt("pieces") ?: 10
                assetReadMultipleGo(applicationContext.assets, pieces)
            }

            Action.OPEN_ONE_GO -> {
                openOneGo(dataDir)
            }

            Action.OPEN_NO_STAT_ONE_GO -> {
                openNoStatOneGo(dataDir)
            }

            Action.FILE_READ_ONE_GO -> {
                fileReadOneGo(dataDir)
            }

            Action.FILE_READ_MULTIPLE_GO -> {
                val pieces = intent.extras?.getInt("pieces") ?: 10
                fileReadMultipleGo(dataDir, pieces)
            }

            Action.STREAM_FILE_READ_ONE_GO -> {
                streamFileReadOneGo(dataDir)
            }

            Action.STREAM_FILE_READ_MULTIPLE_GO -> {
                val pieces = intent.extras?.getInt("pieces") ?: 10
                streamFileReadMultipleGo(dataDir, pieces)
            }

            Action.FOPEN_ONE_GO -> {
                fopenOneGo(dataDir)
            }

            else -> {
                Log.e(TAG, "Unknown action")
            }
        }
    }

    private external fun stringFromJNI(): String

    private external fun init(assetManager: AssetManager, dataDir: String)
    private external fun assetReadOneGo(assetManager: AssetManager)
    private external fun assetReadMultipleGo(assetManager: AssetManager, n: Int)
    private external fun openOneGo(dataDir: String)
    private external fun openNoStatOneGo(dataDir: String)
    private external fun fileReadOneGo(dataDir: String)
    private external fun fileReadMultipleGo(dataDir: String, n: Int)
    private external fun streamFileReadOneGo(dataDir: String)
    private external fun streamFileReadMultipleGo(dataDir: String, n: Int)
    private external fun fopenOneGo(dataDir: String)


    companion object {
        private val TAG = MainActivity::class.java.simpleName

        init {
            val startTime = System.currentTimeMillis()

            System.loadLibrary("example_assetpack")

            val endTime = System.currentTimeMillis()
            val duration = endTime - startTime
            Log.d(TAG, "loadLibrary duration: $duration ms")

        }
    }

    enum class Action {
        INIT,
        ASSET_READ_ONE_GO,
        ASSET_READ_MULTIPLE_GO,
        OPEN_ONE_GO,
        OPEN_NO_STAT_ONE_GO,
        FILE_READ_ONE_GO,
        FILE_READ_MULTIPLE_GO,
        STREAM_FILE_READ_ONE_GO,
        STREAM_FILE_READ_MULTIPLE_GO,
        FOPEN_ONE_GO;

        companion object {
            fun fromIntent(intent: Intent): Action? {
                return when (intent.action) {
                    "com.example.assetpack.action.INIT" -> INIT
                    "com.example.assetpack.action.ASSET_READ_ONE_GO" -> ASSET_READ_ONE_GO
                    "com.example.assetpack.action.ASSET_READ_MULTIPLE_GO" -> ASSET_READ_MULTIPLE_GO
                    "com.example.assetpack.action.OPEN_ONE_GO" -> OPEN_ONE_GO
                    "com.example.assetpack.action.OPEN_NO_STAT_ONE_GO" -> OPEN_NO_STAT_ONE_GO
                    "com.example.assetpack.action.FILE_READ_ONE_GO" -> FILE_READ_ONE_GO
                    "com.example.assetpack.action.FILE_READ_MULTIPLE_GO" -> FILE_READ_MULTIPLE_GO
                    "com.example.assetpack.action.STREAM_FILE_READ_ONE_GO" -> STREAM_FILE_READ_ONE_GO
                    "com.example.assetpack.action.STREAM_FILE_READ_MULTIPLE_GO" -> STREAM_FILE_READ_MULTIPLE_GO
                    "com.example.assetpack.action.FOPEN_ONE_GO" -> FOPEN_ONE_GO
                    else -> null
                }
            }
        }
    }
}
