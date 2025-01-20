import { icons } from './vicon';
import { fr } from 'vuetify/locale';
import { createVuetify } from 'vuetify'

import 'vuetify/styles'

import { mdi } from 'vuetify/iconsets/mdi-svg'


export default createVuetify({
	locale: {
		messages: { fr },
		locale: 'fr',
	},
	icons: {
		defaultSet: 'mdi',
		aliases: {
			clear: icons.mdiCloseCircle,
			radioOn: icons.mdiRadioboxMarked,
			radioOff: icons.mdiRadioboxBlank,
			expand: icons.mdiChevronDown,
			dropdown: icons.mdiMenuDown
		},
		sets: {
			mdi,
		},
	},
});