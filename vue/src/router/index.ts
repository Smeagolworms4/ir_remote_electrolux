import { createRouter, createWebHashHistory, RouteRecordRaw } from 'vue-router';
import HomeView from '../views/HomeView.vue';
import WifiView from '../views/WifiView.vue';
import MqttView from '../views/MqttView.vue';
import HotteView from '../views/HotteView.vue';

const routes: Array<RouteRecordRaw> = [
	{
		path: '/',
		name: 'home',
		component: HomeView
	},
	{
		path: '/wifi',
		name: 'wifi',
		component: WifiView
	},
	{
		path: '/mqtt',
		name: 'mqtt',
		component: MqttView
	},
	{
		path: '/hotte',
		name: 'hotte',
		component: HotteView
	},
]

const router = createRouter({
	history: createWebHashHistory(),
	routes
})

export default router
