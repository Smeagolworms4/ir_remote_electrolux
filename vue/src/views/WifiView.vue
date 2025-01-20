<template>
	<v-card>

		<v-card-title>
			Wifi Configuration
		</v-card-title>

		<v-card-text>

			<v-row>
				<v-col v-if="error" cols="12">
					<v-alert type="error">{{ error }}</v-alert>
				</v-col>
				<v-col v-if="success" cols="12">
					<v-alert type="success">{{ success }}</v-alert>
				</v-col>

				<v-col cols="12" md="8" lg="9">

					<v-progress-circular v-if="loadingInfo" indeterminate color="primary" size="100" width="7" />

					<v-form v-model="valid" v-else @submit.prevent="send">

						<p v-if="!value.apEnabled && !value.stEnabled" class="text-error">
							You must enabled AP mode or Station mode
						</p>
						
						<v-card>
							<v-card-title>
								AP Mode
							</v-card-title>
							<v-card-text>

								<v-switch label="Enabled" color="primary" v-model="value.apEnabled" />
								
								<v-text-field
									v-model="value.apSsid"
									:rules="value.apEnabled ? [
										$validators.required()
									] : []"
									:clear-icon="$icons.mdiCloseCircle"
									label="SSID"
									variant="outlined"
									validate-on="blur"
									clearable
								/>

								<v-text-field
									v-model="value.apPassword"
									:type="showPassword ? 'text' : 'password'"
									:append-inner-icon="showPassword ? $icons.mdiEyeOff : $icons.mdiEye"
									:clear-icon="$icons.mdiCloseCircle"
									@click:append-inner="showPassword = !showPassword"
									label="Password"
									variant="outlined"
									validate-on="blur"
									clearable
								/>
							</v-card-text>
						</v-card>
						<v-card>
							<v-card-title>
								Station Mode
							</v-card-title>
							<v-card-text>
								
								<v-switch label="Enabled" color="primary" v-model="value.stEnabled" />
								
								<v-text-field
									v-model="value.stSsid"
									:rules="value.stEnabled ? [
										$validators.required()	
									] : []"
									:clear-icon="$icons.mdiCloseCircle"
									label="SSID"
									variant="outlined"
									validate-on="blur"
									clearable
									:disabled="!value.stEnabled"
								/>

								<v-text-field
									v-model="value.stPassword"
									:type="showPassword ? 'text' : 'password'"
									:append-inner-icon="showPassword ? $icons.mdiEyeOff : $icons.mdiEye"
									:clear-icon="$icons.mdiCloseCircle"
									:disabled="!value.stEnabled"
									@click:append-inner="showPassword = !showPassword"
									label="Password"
									variant="outlined"
									validate-on="blur"
									clearable
								/>

								<v-select
									v-model="value.dhcp"
									:items="[
										{ value: 'DHCP', title: 'DHCP' },
										{ value: 'DHCP_DNS', title: 'DHCP without DNS' },
										{ value: 'STATIC', title: 'Static' },
									]"
									label="IP Mode"
									item-value="value"
									item-title="title"
									variant="outlined"
									:disabled="!value.stEnabled"
								/>

								
								<template v-if="value.stEnabled">
									<template v-if="value.dhcp == 'STATIC'">

										<v-text-field
											v-model="value.ip"
											:rules="[
												$validators.required(),
												$validators.ipv4(),
											]"
											label="IP"
											placeholder="0.0.0.0"
											variant="outlined"
											validate-on="blur"
											clearable
											@keydown="validateIp"
											@keyup="validateIp"
										/>

										<v-text-field
											v-model="value.mask"
											:rules="[
												$validators.required(),
												$validators.ipv4(),
											]"
											label="Mask"
											placeholder="255.255.255.0"
											variant="outlined"
											validate-on="blur"
											clearable
											@keydown="validateIp"
											@keyup="validateIp"
										/>

										<v-text-field
											v-model="value.gateway"
											:rules="[
												$validators.ipv4(),
											]"
											label="Gateway"
											placeholder="255.255.255.0"
											variant="outlined"
											validate-on="blur"
											clearable
											@keydown="validateIp"
											@keyup="validateIp"
										/>

									</template>

									<template v-if="value.dhcp != 'DHCP'">

										<v-text-field
											v-model="value.dns1"
											:rules="[
												$validators.ipv4(),
											]"
											label="DNS 1"
											placeholder="0.0.0.0"
											variant="outlined"
											validate-on="blur"
											clearable
											@keydown="validateIp"
											@keyup="validateIp"
										/>

										<v-text-field
											v-model="value.dns2"
											:rules="[
												$validators.ipv4(),
											]"
											label="DNS 2"
											placeholder="0.0.0.0"
											variant="outlined"
											validate-on="blur"
											clearable
											@keydown="validateIp"
											@keyup="validateIp"
										/>
									</template>
								</template>

							<v-divider class="mb-4"/>

							<div style="text-align: right">
								<v-btn
									type="submit"
									color="primary"
									:disabled="!valid || (!value.apEnabled && !value.stEnabled)"
									:loading="loadingSend"
								>Save</v-btn>
							</div>

							</v-card-text>

						</v-card>

					</v-form>

				</v-col>
				<v-col cols="12" md="4" lg="3">
					<v-card>

						<v-card-title>
							Wifi list:
							<v-spacer />
							<v-btn color="grey-darken-3" size="small" :loading="loadingScan" @click="refresh()" >Refresh</v-btn>
						</v-card-title>

						<v-card-text>
							<template v-for="(wifi, key) of scan" :key="key">

								<v-card class="mb-3" v-ripple style="cursor: pointer;" @click="value.stSsid = wifi.ssid">
									<v-card-title>
										{{ wifi.ssid }}
									</v-card-title>
									<v-card-text>

										<div>
											<wifi-quality :quality="wifi.quality" />&nbsp;Quality: {{ wifi.quality }}
										</div>

										<div>
											<v-icon>{{ $icons.mdiLock }}</v-icon>&nbsp;Encryption: {{ wifi.encryption_type }}
										</div>
										
										<div style="text-align: right">
											<v-btn color="primary" size="small" >Select</v-btn>
										</div>
									</v-card-text>

								</v-card>

							</template>
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
		name: 'WifiView',

		components: {
			WifiQuality
		},

		data: () => ({
			loadingInfo: false,
			loadingScan: false,
			loadingSend: false,
			error: "",
			success: "",
			scan: [] as any[]
			
			,
			showPassword: false,
			valid: false,
			value: {
				dhcp: "DHCP",
				apEnabled: false,
				apSsid: "",
				apPassword: "",
				stEnabled: false,
				stSsid: "",
				stPassword: "",
				ip: "",
				mask: "",
				gateway: "",
				dns1: "",
				dns2: "",
			}
		}),

		async mounted() {
			this.refresh();
			this.loadingInfo = true;
			try {
				this.value = await (await fetch('/api/wifi/config')).json();
			} catch(e) {
				this.error = "Error on load current wifi configutation";
				console.error(e);
			}
			this.loadingInfo = false;
		},
		
		watch: {
			'value.dhcp'() {
				this.clearValue();
			},
			'value.stEnabled'() {
				this.clearValue();
			}
		},

		methods: {
			async refresh() {
				this.loadingScan = true;
				try {
					this.scan = await (await fetch('/api/wifi/scan')).json();
					this.error = "";
					this.success = "";
				} catch(e) {
					this.error = "Error on load scan wifi";
					console.error(e);
				}
				this.loadingScan = false;
			},

			async send() {
				this.error = "";
				this.success = "";
				this.loadingSend = true;
				try {
					this.value = await (await fetch('/api/wifi/config', {
						method: 'POST',
						body: JSON.stringify(this.value)
					}).then(r => { if (r.ok) return r; throw r; })).json();
					this.error = "";
					this.success = "Saved with success.";
				} catch(e) {
					this.error = "Error on save wifi configuration";
					console.error(e);
				}
				this.loadingSend = false;
			},

			validateIp() {
				this.value.ip      = this.value.ip     .replace(/[^\d\.]/g, '');
				this.value.mask    = this.value.mask   .replace(/[^\d\.]/g, '');
				this.value.gateway = this.value.gateway.replace(/[^\d\.]/g, '');
				this.value.dns1    = this.value.dns1   .replace(/[^\d\.]/g, '');
				this.value.dns2    = this.value.dns2   .replace(/[^\d\.]/g, '');				
			},
			
			clearValue() {
				if (!this.$validators.ipv4()(this.value.ip) === true) {
					this.value.ip = '';
				}
				if (!this.$validators.ipv4()(this.value.mask) === true) {
					this.value.mask = '';
				}
				if (!this.$validators.ipv4()(this.value.gateway) === true) {
					this.value.gateway = '';
				}
				if (!this.$validators.ipv4()(this.value.dns1) === true) {
					this.value.dns1 = '';
				}
				if (!this.$validators.ipv4()(this.value.dns2) === true) {
					this.value.dns2 = '';
				}
			}
		}
	});
</script>

<style>
	* {
		font-family: arial, sans-serif;
	}
</style>
