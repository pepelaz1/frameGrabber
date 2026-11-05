# FrameGrabber — Windows camera capture and serial trigger

Two Windows desktop implementations for previewing a camera and saving frames. The C# WinForms app uses DirectShow for video capture and monitors a serial port for an external trigger. The repository also contains a C++/Qt application and its `Streams` capture library.

## Applications

- `FrameGrabberApp/`: C# WinForms application, originally targeting .NET Framework 4.5. It enumerates video devices, previews the selected source, takes a picture manually, and handles a configured COM port.
- `HauppageApp/`: C++/Qt application using `Streams` and Qt Serial Port. It targets an older Qt 5.5 / MSVC 2010 environment.

## Verified local run

On Windows with Visual Studio 2022, the C# project built as x86 Release when retargeted at build time to .NET Framework 4.8. The application launched, detected an installed virtual camera, and saved a **640 × 480 PNG** after a manual **Take picture** action. A physical capture device and the serial-port trigger were unavailable, so that end-to-end hardware path remains untested. The Qt application's legacy runtime dependencies also need further work before its UI can be demonstrated reliably on this machine.

The manual capture produced a 640 × 480 PNG from the virtual camera. A clean application screenshot will be added after another local run with a short output path.

## Reproduce the C# build

Open `FrameGrabberApp/FrameGrabberApp.sln` in Visual Studio with .NET desktop development installed, or use MSBuild with `Configuration=Release`, `Platform=x86`, and `TargetFrameworkVersion=v4.8`. Choose an existing writable output directory in the UI before capturing. The capture source must be a DirectShow-compatible camera or virtual camera.
