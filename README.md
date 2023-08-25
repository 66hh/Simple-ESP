# Simple ESP
A simple open-source project demonstrating the possibility of implementing ESP for Java Minecraft (from 1.0 to 1.12.2) using interceptions of only OpenGL functions.

### Explanation
Java Minecraft uses the OpenGL library to render graphics. The game does not tell the library what specific game object is currently being drawn, but we can determine this by some signs. For example, the `glTranslatef(0.5f, 0.4375f, 0.9375f)` function is always called before the chest is drawn. By intercepting the function and [comparing] its parameters, we can indirectly determine that at the moment the game is going to draw the chest. You cannot get the game coordinates of the object, but you can [save] the current (at the time of the function call) matrixes settings and [use] them later to draw something of your own in the same place.

### Attention
The provided source code is offered "as is," without any warranties or guarantees of its fitness for a particular purpose or functionality. The user assumes all risks associated with its use, modification, or distribution. The author shall not be liable for any damages, losses, or issues arising from the use of the provided code.

### Compilation
- Clone this repository in any way you like.
- Open the **Detours** solution file in Visual Studio IDE.
  - Select your the target platform.
  - Press `Ctrl + Shift + B` to compile.
- Open the **Simple ESP** solution file in Visual Studio IDE.
  - Select your the target platform.
  - Press `Ctrl + Shift + B` to compile.

All compiled open-source releases available in our telegram [channel].

### Usage
- Open any DLL-injector **as administrator**.
- Find the Java Minecraft process.
- Inject the `Simple ESP.dll` into process.

### Preview
![Image](../assets/Image.png?raw=true)

### Support
If you want to support me, you can subscribe to my telegram [channel] or send a voluntary donation to my next BTC wallet
```
173UHoNaXZtersBwdgsBh3eTrbXVPijQJ7
```

[channel]: <https://t.me/aurenex>
[comparing]: <../master/Simple%20ESP/DllMain.cpp#L184C2-L184C2>
[save]: <../master/Simple%20ESP/DllMain.cpp#L22>
[use]: <../master/Simple%20ESP/DllMain.cpp#L147>
[LICENSE]: <../master/LICENSE.txt>
