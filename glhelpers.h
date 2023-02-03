#pragma once
/*
	OpenGL Helper Functions
	Copyright (c) 2020, 2023, Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#ifdef __cplusplus
extern "C" {
#endif

const char* glhelper_debug_source_str(GLenum source);
const char* glhelper_debug_severity_str(GLenum severity);
const char* glhelper_debug_type_str(GLenum type);

int glhelper_install_debug_callback(GLDEBUGPROC callback, void *cbdata);

#ifdef EUTILS_GLHELPERS_IMPLEMENTATION

const char* glhelper_debug_source_str(GLenum source) {
	const char *msg = "<UNKNOWN SOURCE>";
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			msg = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			msg = "WINDOW_SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			msg = "SHADER_COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			msg = "THIRD_PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			msg = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			msg ="OTHER";
			break;
	}
	return msg;
}

const char* glhelper_debug_severity_str(GLenum severity) {
	const char *msg = "<UNKNOWN SEVERITY>";
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			msg = "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			msg = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			msg = "HIGH";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			msg = "NOTIFICATION";
			break;
	}
	return msg;
}

const char* glhelper_debug_type_str(GLenum type) {
	const char *msg = "<UNKNOWN TYPE>";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			msg = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			msg = "DEPRECATED_BEHAVIOUR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			msg = "UNDEFINED_BEHAVIOUR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			msg = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			msg = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_MARKER:
			msg = "MARKER";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			msg = "PUSH_GROUP";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			msg = "POP_GROUP";
			break;
		case GL_DEBUG_TYPE_OTHER:
			msg = "OTHER";
			break;
	}
	return msg;
}

static void glhelper_default_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *data) {
#if 0
	// Ignore some nVidia spam about buffer object memory type.
	if (source == GL_DEBUG_SOURCE_API && severity == GL_DEBUG_SEVERITY_NOTIFICATION && type == GL_DEBUG_TYPE_OTHER)
		return;
#endif
	fprintf(stderr, "GL CALLBACK:%s source=%s (%x), type=%s (%x), severity=%s (%x), message:\n>>%s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "**ERROR**" : ""),
		glhelper_debug_source_str(source), source,
		glhelper_debug_type_str(type), type,
		glhelper_debug_severity_str(severity), severity,
		message);
}

int glhelper_install_debug_callback(GLDEBUGPROC callback, void *cbdata) {
	if (!callback)
		callback = glhelper_default_error_callback;

	int context_flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, // source, type, severity
			0, NULL, GL_TRUE);
		glDebugMessageCallback(callback, cbdata);
		return 0;
	}

	return -1;
}

#endif

#ifdef __cplusplus
}
#endif
