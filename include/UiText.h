#pragma once

// Marks a Turkish message that is shown in the web portal. The portal uses the
// exact text as the key of its translation table (web/portal-i18n.js), and
// tools/generate_portal_compressed.mjs refuses to build the portal while any
// UI_TEXT message is missing a translation.
#define UI_TEXT(text) (text)
