# gif-author


# TODO List

- typedef Glib::RefPtr<VideoFrame>, because it's getting painful
- std::list<Glib::RefPtr<VideoFrame>> Video::extract_frames(...) in order to get selected frames
- Improve looping before frame selection
- Frame settings object to store output settings like visibility, colortable etc
- Zoomable ImageArea
- Image preview for side by side comparisons
- Filmstrip with frame enable/disable
- Basic Processing chain:
  - (background detection), (stabilise), rotate, crop, scale, palette creation,
    dither, gif output.

