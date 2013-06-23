TEMPLATE = subdirs

!contains(CONFIG, no_widgets):!contains(CONFIG, no_codeeditor):SUBDIRS+=simple-editor
