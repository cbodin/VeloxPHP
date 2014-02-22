// Fallback to local copy of jquery
if (!window.jQuery) {
    document.write('<script src="' + Velox.Settings.jqueryPath +'"><\/script>');
}

// To avoid conflict with other libraries, remove the global $
// variable
$.noConflict();
