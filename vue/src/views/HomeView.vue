<template>
	<v-card>

		<v-card-title>
			ESP 8266 - Water Measurement - Status
		</v-card-title>

		<v-card-text>
			
			<v-row>
				<v-col v-if="error" cols="12">
					<span class="error--text">{{ error }}</span>
				</v-col>

				<v-col cols="12" md="4">

					<v-card>
						<v-card-title>
							Device Status
						</v-card-title>
						<v-card-text>
							<v-progress-circular v-if="loadingDevice" indeterminate color="primary" size="70" width="7" />
							<v-table v-else>
								<tr><th>Memory free: </th><td>{{ device.memory_free }} Bytes</td></tr>
								<tr><th>Max free block: </th><td>{{ device.max_free_block }} Bytes</td></tr>
								<tr><th>Fragmentation: </th><td>{{ device.fragmentation }}%</td></tr>
							</v-table>
						</v-card-text>

					</v-card>

				</v-col>

				<v-col cols="12" md="4">

					<v-card>
						<v-card-title>
							Wifi Status
						</v-card-title>
						<v-card-text>
							<v-progress-circular v-if="loadingWifi" indeterminate color="primary" size="70" width="7" />
							<v-table v-else>
								<tr><th>Mode: </th><td>{{ wifi.mode }}</td></tr>
								<tr><th>IP: </th><td>{{ wifi.ip }}</td></tr>
								<tr><th>Mask: </th><td>{{ wifi.mask }}</td></tr>
								<tr><th>Gateway: </th><td>{{ wifi.gateway }}</td></tr>
								<tr><th>DNS: </th><td>{{ wifi.dsn }}</td></tr>
								<tr><th>Quality: </th><td><wifi-quality :quality="wifi.quality" />&nbsp;{{ wifi.quality }}</td></tr>
							</v-table>
						</v-card-text>

					</v-card>

				</v-col>

				<v-col cols="12" md="4">

					<v-card>
						<v-card-title>
							Mqtt Status
						</v-card-title>
						<v-card-text>
							<v-progress-circular v-if="loadingMqtt" indeterminate color="primary" size="70" width="7" />
							<v-table v-else>
								<tr><th>Status: </th><td>{{ mqtt.status }}</td></tr>
							</v-table>
						</v-card-text>

					</v-card>

				</v-col>
			</v-row>

		</v-card-text>

  </v-card>
</template>


<script lang="ts">
	import { defineComponent } from 'vue';
	import WifiQuality from '../components/WifiQuality.vue';

	export default defineComponent({
		name: 'HomeView',

		components: {
			WifiQuality
		},

		data: () => ({
			mounted: false,
			error: "",
			loadingWifi: true,
			loadingDevice: true,
			loadingMqtt: true,
			wifi: {
				mode: "",
				ip: "",
				mask: "",
				gateway: "",
				quality: 0,
			},
			mqtt: {
				status: "INTIALIZE",
			},
			device: {
				fragmentation: 0,
				max_free_block: 0,
				memory_free: 0,
			}
		}),

		async mounted() {
			this.mounted = true;
			this.refresh();
		},
		
		unmounted() {
			this.mounted = false;
		},

		methods: {
			async refresh(loading: boolean = true) {
				try {
					await Promise.all([
						this.refreshWifi(loading),
						this.refreshDevice(loading),
						this.refreshMqtt(loading),
					]);
				} catch(e) {
					console.error(e);
				}
				setTimeout(() => this.mounted && this.refresh(false), 2000);
				
			},
			async refreshWifi(loading: boolean = true) {
				this.error = "";
				this.loadingWifi = loading;
				try {
					this.wifi = await (await fetch('/api/wifi/status')).json();
				} catch(e) {
					this.error = "Error on load wifi status";
					console.error(e);
				}
				this.loadingWifi = false;
			},
			async refreshDevice(loading: boolean = true) {
				this.error = "";
				this.loadingDevice = loading;
				try {
					this.device = await (await fetch('/api/status')).json();
				} catch(e) {
					this.error = "Error on load device status";
					console.error(e);
				}
				this.loadingDevice = false;
			},
			async refreshMqtt(loading: boolean = true) {
				this.error = "";
				this.loadingMqtt = loading;
				try {
					this.mqtt = await (await fetch('/api/mqtt/status')).json();
				} catch(e) {
					this.error = "Error on load mqtt status";
					console.error(e);
				}
				this.loadingMqtt = false;
			},
		}
	});
</script>

<style>
	* {
		font-family: arial, sans-serif;
	}
</style>
