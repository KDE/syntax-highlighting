<!--
    SPDX-FileCopyrightText: 2024 James Zuccon
    SPDX-License-Identifier: MIT
-->
<!--
    This uses HTML syntax as a base with some customizations for handling Vue-Template-specific syntax.
    It uses the following third-party scripts.
    Script:
    - JS (default - no lang attribute)
    - Typescript (lang="ts")
    Style:
    - CSS (default - no lang attribute)
    - SASS (lang="sass")
    - SCSS (lang="scss")
    NOTE: The tests below test attributes before/after the lang="..." attribute as doing this requires pushing a new context.
          In practice, a Vue Template would generally only contain zero or one <template>, <script> or <style> blocks.
-->

<!-- Template without attributes -->
<template>
    <div>
        <!-- Vue event-shorthand should support "@" prefix -->
        <p @click="test">Testing HTML</p>
        <p>{{ someVar }}</p>
        <!-- Important to test this as it is used at root also -->
        <template>
            Nested template tag
        </template>
    </div>
</template>

<!-- Template with attributes -->
<template loneAttribute attribute="value">
    <div>
        <!-- Vue event-shorthand should support "@" prefix -->
        <p @click="test">Testing HTML</p>
        <p>{{ someVar }}</p>
        <!-- Important to test this as it is used at root also -->
        <template>
            Nested template tag
        </template>
    </div>
</template>

<!-- Style without attributes -->
<!-- This references the syntax highlighting file for CSS. -->
<style>
	body {
		font: 100% Helvetica, sans-serif;
		color: #333;
	}
</style>

<!-- Style with attributes -->
<!-- This references the syntax highlighting file for CSS. -->
<style loneAttribute attribute="value">
	body {
		font: 100% Helvetica, sans-serif;
		color: #333;
	}
</style>

<!-- Style lang="sass" without attributes -->
<!-- This references the syntax highlighting file for SASS. -->
<style lang="sass">
	// single-line comment to make sure it's SASS and not CSS
	$font-stack: Helvetica, sans-serif
	$primary-color: #333

	body
		font: 100% $font-stack
		color: $primary-color
</style>

<!-- Style lang="sass" with attributes -->
<!-- This references the syntax highlighting file for SASS. -->
<style beforeLangLoneAttribute beforeLangAttribute="asd" lang="sass" afterLangLoneAttribute afterLangAttribute="asd">
	// single-line comment to make sure it's SASS and not CSS
	$font-stack: Helvetica, sans-serif
	$primary-color: #333

	body
		font: 100% $font-stack
		color: $primary-color
</style>

<!-- Style lang="scss" without attributes -->
<!-- This references the syntax highlighting file for SCSS. -->
<style lang="scss">
	// single-line comment to make sure it's SCSS and not CSS
	$font-stack: Helvetica, sans-serif
	$primary-color: #333

	body {
		font: 100% $font-stack;
		color: $primary-color;
    }
</style>

<!-- Style lang="scss" with attributes -->
<!-- This references the syntax highlighting file for SCSS. -->
<style beforeLangLoneAttribute beforeLangAttribute="asd" lang="scss" afterLangLoneAttribute afterLangAttribute="asd">
	// single-line comment to make sure it's SCSS and not CSS
	$font-stack: Helvetica, sans-serif
	$primary-color: #333

	body {
		font: 100% $font-stack;
		color: $primary-color;
    }
</style>

<!-- Script without attributes -->
<!-- This references the syntax highlighting file for Javascript. -->
<script>
// Single-line comment
const a = 'abc';
</script>

<!-- Script with attributes -->
<!-- This references the syntax highlighting file for Javascript. -->
<script loneAttribute attribute="value">
// Single-line comment
const a = 'abc';
</script>

<!-- Script lang="ts" without attributes -->
<!-- This references the syntax highlighting file for Typescript. -->
<script lang="ts">
// Single-line comment
const a = 'abc';

// Typescript
interface SomeInterface {
    value: string;
}
</script>

<!-- Script lang="ts" with attributes -->
<!-- This references the syntax highlighting file for Typescript. -->
<script beforeLangLoneAttribute beforeLangAttribute="asd" lang="ts" afterLangLoneAttribute afterLangAttribute="asd">
// Single-line comment
const a = 'abc';

// Typescript
interface SomeInterface {
    value: string;
}
</script>
