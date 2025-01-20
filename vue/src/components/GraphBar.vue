<template>
	<div class="component-GraphBar">
		<div class="component-GraphBar-bars">
			<div
				v-for="value, key of valueFull"
				:key="key"
				class="component-GraphBar-bar"
				v-tooltip:top.eager="`${value}mL`"
			>
				<div
					:style="{ height: (value / maxValue * 100)+'%' }"
				/>
			</div>
		</div>
	</div>
</template>

<script lang="ts">
	import { defineComponent, Prop } from 'vue';

	export default defineComponent({
		name: 'GraphBar',
		props: {
			values: { type: Array, default: () => [] } as Prop<number[]>
		},
		computed: {
			valueFull(): number[] {
				return [
					...this.values || [],
					...(new Array(Math.max(60 - (this.values || []).length, 0)).fill(0))
				].reverse();
			},
			maxValue(): number {
				return Math.max(this.valueFull.reduce((a, v) => Math.max(a, v), 0), 200);
			}
		}
	});
</script>

<style lang="scss">
	.component-GraphBar {
		height: 350px;
    	padding: 10px;
		border: 1px solid #999;
		border-radius: 8px;
		
		
		&-bars {
			margin: 0px auto;
			background-color: #fff;
			display: flex;
			width: 100%;
			height: 100%;
			background: rgba(0,0,0,0.03);
			border-left: 1px solid #000;
			border-bottom: 1px solid #000;
			padding: 5px;
			
		}
	
		&-bar {
			height: 0px;
			transition: .22s all linear 0s;
			height: 100%;
			flex: 1 1;
			position: relative;
			
			> div {
				position: absolute;
				left: 0;
				bottom: 0;
				height: 0;
				width: 100%;
				background: rgb(55, 119, 255);
				border: 1px solid  rgb(55, 119, 255);
			}
		}
	}
</style>
