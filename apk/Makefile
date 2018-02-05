
all: debug

debug:
	./gradlew assembleDebug

release:
	./gradlew assembleRelease

sign:
	/home/sion/android/sdk/build-tools/25.0.2/zipalign -f -v 4 ./app/build/outputs/apk/app-release-unsigned.apk ./app/build/outputs/apk/sparkle.apk
	/home/sion/android/sdk/build-tools/25.0.2/apksigner sign --ks ~/android/sion.keystore --ks-key-alias sion ./app/build/outputs/apk/sparkle.apk

install: debug
	./gradlew uninstallDebug
	adb install app/build/outputs/apk/app-debug.apk

installRelease:
	./gradlew uninstallRelease
	adb install app/build/outputs/apk/sparkle.apk

clean:
	rm -rf ./build ./app/build ./app/.externalNativeBuild ./.gradle

