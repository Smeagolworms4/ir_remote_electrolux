<template>
	<v-app>
		<v-app-bar app color="primary" dark>
			<v-spacer />
			<v-toolbar-title>ESP 8266 - Hotte - Admin</v-toolbar-title>
			<v-spacer />

			<template #extension>
				<v-tabs fixed-tabs class="w-100">
					<v-tab :to="{ name: 'home' }"><v-icon>{{ $icons.mdiHome }}</v-icon>&nbsp;Home</v-tab>
					<v-tab :to="{ name: 'wifi' }"><v-icon>{{ $icons.mdiWifi }}</v-icon>&nbsp;Wifi</v-tab>
					<v-tab :to="{ name: 'mqtt' }"><v-icon>{{ $icons.mdiAccessPoint }}</v-icon>&nbsp;Mqtt</v-tab>
					<v-tab :to="{ name: 'hotte' }"><v-icon>{{ $icons.mdiFan }}</v-icon>&nbsp;Hotte</v-tab>
				</v-tabs>
			</template>
		</v-app-bar>

		<v-main>
			<v-container>
				<router-view />
			</v-container>
		</v-main>
	</v-app>
</template>

<script lang="ts">
import { defineComponent } from 'vue'
import {
	mdiHome,
} from '@mdi/js';

export default defineComponent({
	name: 'App',
})
</script>


<style lang="scss">
	* {
		box-sizing: border-box;
	}
</style>
