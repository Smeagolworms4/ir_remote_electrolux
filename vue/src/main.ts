import { createApp } from 'vue';
import App from './App.vue';
import router from './router';
import vuetify from './plugins/vuetify';
import vicon from './plugins/vicon';
import validators from './plugins/validators';

createApp(App)
	.use(router)
	.use(vuetify)
	.use(vicon)
	.use(validators)
	.mount('#app')
;