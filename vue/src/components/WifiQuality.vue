<template>
	<v-icon
		:class="
			isNaN(quality) || quality < 25 ? 'text-error' : (
				quality < 50 ? 'text-warning' : (
					quality < 50 ? 'text-info' : 'text-success'
				)
			)
		"
	>{{
		isNaN(quality) || quality === 0 ? $icons.mdiWifiStrengthOutline : (
			quality < 25 ? $icons.mdiWifiStrength1 : (
				quality < 50 ? $icons.mdiWifiStrength2 : (
					quality < 50 ? $icons.mdiWifiStrength3 : $icons.mdiWifiStrength4
				)
			)
		)
	}}</v-icon>
</template>

<script lang="ts">
	import { defineComponent } from 'vue';

	export default defineComponent({
		name: 'WifiQuality',
		props: {
			quality: { type: Number, default: 0 }
		}
	});
</script>
