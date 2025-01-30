// In the asset pack’s build.gradle file:
plugins {
    alias(libs.plugins.asset.pack)
}

assetPack {
    packName = "FileAssets"
    dynamicDelivery {
        deliveryType = "install-time"
    }
}
