import { App } from 'vue';

import { 
	mdiWifi,
	mdiHome,
	mdiRemote,
	mdiWifiStrengthOutline,
	mdiWifiStrength1,
	mdiWifiStrength2,
	mdiWifiStrength3,
	mdiWifiStrength4,
	mdiLock,
	mdiCloseCircle,
	mdiEye,
	mdiEyeOff,
	mdiRadioboxBlank,
	mdiRadioboxMarked,
	mdiChevronDown,
	mdiFan,
	mdiAccessPoint,
	mdiMenuDown,
} from '@mdi/js';


export const icons = {
	mdiWifi,
	mdiRemote,
	mdiHome,
	mdiWifiStrengthOutline,
	mdiWifiStrength1,
	mdiWifiStrength2,
	mdiWifiStrength3,
	mdiWifiStrength4,
	mdiLock,
	mdiCloseCircle,
	mdiEye,
	mdiEyeOff,
	mdiRadioboxBlank,
	mdiRadioboxMarked,
	mdiChevronDown,
	mdiFan,
	mdiAccessPoint,
	mdiMenuDown,
};

export type IconType = typeof icons;

export default {
	install(app: App) {
		app.config.globalProperties.$icons = icons;
	}
};


declare module '@vue/runtime-core' {
	interface ComponentCustomProperties {
		$icons: IconType;
	}
}
