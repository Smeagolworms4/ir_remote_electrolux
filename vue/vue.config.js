// vue.config.js

/**
 * @type {import('@vue/cli-service').ProjectOptions}
 */
module.exports = {
    productionSourceMap: false,

    chainWebpack: config => {
		config.optimization.delete('splitChunks');
		config.plugins.delete('preload');
	},

    css: {
		extract: {
			filename: '[name].css',
			chunkFilename: '[name].css',
		},
	},

    configureWebpack: {
		output: {
			filename: '[name].js',
			chunkFilename: '[name].js',
		}
	},

	transpileDependencies: [
		'vuetify'
	],
	
    pluginOptions: {
      vuetify: {
		}
    },
	
	devServer: {
		proxy: {
			'/api': {
				target: process.env.PROXY_URL || 'http://192.168.4.1',
				ws: true,
				changeOrigin: true,
				onProxyReq: (proxyReq, req, res) => {
					console.log(`[DEV SERVER] - ${req.method}:${req.url}`);
				}
			}
		}
	}
};
