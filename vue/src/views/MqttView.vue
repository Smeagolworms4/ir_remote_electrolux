<template>
	<v-card>

		<v-card-title>
			MQTT Configuration
		</v-card-title>

		<v-card-text>

			<v-row>
				<v-col v-if="error" cols="12">
					<v-alert type="error">{{ error }}</v-alert>
				</v-col>
				<v-col v-if="success" cols="12">
					<v-alert type="success">{{ success }}</v-alert>
				</v-col>

				<v-col cols="12">

					<v-progress-circular v-if="loadingInfo" indeterminate color="primary" size="100" width="7" />

					<v-form v-model="valid" v-else @submit.prevent="send">

						<v-switch label="Enabled" color="primary" v-model="value.enable" />
						
						<v-text-field
							v-model.trim="value.host"
							:rules="value.enable && !value.host ? [
								$validators.required()
							] : []"
							:clear-icon="$icons.mdiCloseCircle"
							label="Host"
							variant="outlined"
							validate-on="blur"
							clearable
						/>
						
						<v-text-field
							v-model.number="value.port"
							type="number"
							:rules="value.enable && value.host ? [
								$validators.required()
							] : []"
							:clear-icon="$icons.mdiCloseCircle"
							label="Port"
							variant="outlined"
							validate-on="blur"
							clearable
						/>

						<v-text-field
							v-model="value.login"
							:clear-icon="$icons.mdiCloseCircle"
							label="Username"
							variant="outlined"
							clearable
						/>
						
						<v-text-field
							v-model="value.password"
							:type="showPassword ? 'text' : 'password'"
							:append-inner-icon="showPassword ? $icons.mdiEyeOff : $icons.mdiEye"
							:clear-icon="$icons.mdiCloseCircle"
							@click:append-inner="showPassword = !showPassword"
							label="Password"
							variant="outlined"
							clearable
						/>

						<v-text-field
							v-model.trim="value.haDiscovery"
							:clear-icon="$icons.mdiCloseCircle"
							label="Home assistant Discovery (if empty is disabled)"
							variant="outlined"
							clearable
						/>
						

						<v-divider class="mb-4"/>

						<div style="text-align: right">
							<v-btn
								type="submit"
								color="primary"
								:loading="loadingSend"
							>Save</v-btn>
						</div>


					</v-form>

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
			loadingSend: false,
			error: "",
			success: "",
			scan: [] as any[]
			
			,
			showPassword: false,
			valid: false,
			value: {
				enable: false,
				host: "",
				port: 1883,
				login: "",
				password: "",
				haDiscovery: "homeassistant",
			}
		}),

		async mounted() {
			this.loadingInfo = true;
			try {
				this.value = await (await fetch('/api/mqtt/config')).json();
			} catch(e) {
				this.error = "Error on load current wifi configutation";
				console.error(e);
			}
			this.loadingInfo = false;
		},
		
		methods: {
			async send() {
				this.error = "";
				this.success = "";
				this.loadingSend = true;
				try {
					this.value = await (await fetch('/api/mqtt/config', {
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
		}
	});
</script>

<style>
	* {
		font-family: arial, sans-serif;
	}
</style>
