<template>
	<div>
		
		<v-alert v-if="error" type="error" class="mb-2">{{ error }}</v-alert>
		<v-alert v-if="success" type="success" class="mb-2">{{ success }}</v-alert>
		<div v-else style="height: 68px;"/>
		
		<v-card title="Last send signal:">
			<v-card-text>
				<strong>{{ status?.last || 'NO SIGNAL' }}</strong>
			</v-card-text>
		</v-card>
		<v-card class="mt-4" title="Actions">
			<v-card-text>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('light/on')">Send Light On</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('light/off')">Send Light Off</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('fan/0')">Send Fan Off</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('fan/1')">Send Fan 1</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('fan/2')">Send Fan 2</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('fan/3')">Send Fan 3</v-btn><br/>
				<v-btn class="mb-2" color="primary" :loading="loadingSend" @click="send('fan/4')">Send Fan 4</v-btn><br/>
			</v-card-text>
		</v-card>
	</div>
</template>

<script lang="ts">
	import { defineComponent } from 'vue';

	export default defineComponent({
		name: 'Hotte',

		data: () => ({
			status: null as any,
			mounted: false,
			error: "",
			success: "",
			loadingSend: false,
		}),

		mounted() {
			this.mounted = true;
			this.refresh();
		},
		
		unmounted() {
			this.mounted = false;
		},

		methods: {
			async refresh() {
				try {
					this.error = "";
					this.status = await (await fetch('/api/hotte/last')).json();
				} catch(e) {
					console.error(e);
					this.success = "";
					this.error = "Error on save water-m configuration";
				}
				if (this.mounted) {
					setTimeout(() => this.refresh(), 1000);
				}
			},
			async send(signal: any) {
				this.error = "";
				this.success = "";
				this.loadingSend = true;
				try {
					await (await fetch(`/api/hotte/${signal}`)).json();
					this.success = "Signal send success";
				} catch(e) {
					this.error = "Error on send signal";
					console.error(e);
				}
				this.success = "Saved with success.";
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
