
代码功能：本代码是《PBRT专业知识理论与代码实战》部分的附加工具，用以将通用的模型文件(.obj、.blend、.max等)转换为我们搭建的渲染器更容易读取的模型文件。

由于网上下载的很多3D模型的纹理等索引都是绝对路径，所以转换中可能会找不到纹理（用其他软件打开模型也同样找不到纹理）。

开发环境：Visual 2015 x64位。如果更换环境，需要重新去官网下载3rdLib中的OpenGL和Assimp库。

OpenGL: [ https://www.glfw.org/download.html ]
Assimp：[ http://assimp.org/]

3rdLib目录下是Rattler用到的第三方库，在CMake之前需要解压到3rdLib所在目录。


