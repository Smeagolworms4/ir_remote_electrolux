import { App } from 'vue';
import { Validators } from './Validators';

export const validators = new Validators();

export default {
    install(app: App) {
        app.config.globalProperties.$validators = validators;
    }
};


declare module '@vue/runtime-core' {
    interface ComponentCustomProperties {
        $validators: Validators;
    }
}