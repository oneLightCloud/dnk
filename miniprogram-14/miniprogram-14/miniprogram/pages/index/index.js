// index.js
// const app = getApp()
const { envList } = require('../../envList.js');

var bmap = require('../../libs/bmap-wx.js');
var wxMarkerData =[]; //定位成功回调函数

Page({
  data: {
    ak:"xOGCUxvZtUfQdNEidzhEG8ahl52oLHuk",//申请的我的ak 百度地图
    location:"",
    temp:"",
    tianqi:"",
    xian:"",
    shi:"",
    humi:"",
    showUploadTip: false,
    powerList: [{
      title: '数据库',
      tip: '安全稳定的文档型数据库',
      showItem: false,
      item: [{
        title: '创建集合',
        page: 'createCollection'
      }, {
        title: '更新记录',
        page: 'updateRecord'
      }, {
        title: '查询记录',
        page: 'selectRecord'
      }, {
        title: '聚合操作',
        page: 'sumRecord'
      }]
    }, {
      title: '云存储',
      tip: '自带CDN加速文件存储',
      showItem: false,
      item: [{
        title: '上传文件',
        page: 'uploadFile'
      }]
    }, 
    ],
    envList,
    selectedEnv: envList[0],
    haveCreateCollection: false
  },

  onLoad:function(){
    var that=this;
    var BMap = new bmap.BMapWX({
      ak:that.data.ak
    });
var fail = function(data){
  console.log(data);
};
var success = function(data){
  console.log(data);
  wxMarkerData = data.wxMarkerData;
  console.log(wxMarkerData[0].desc)
  that.setData({
    location:wxMarkerData[0].desc
  })
}
BMap.regeocoding({
fail:fail,
success:success
})
  },

getweather: function(e){
var that=this
wx. getLocation({
type: "wgs84",
success (res) {
const latitude = res.latitude
const longitude = res.longitude
const key='fc5ce28d2cb74a71bca7e21826d33794'

wx.request({
url: 'https://geoapi.qweather.com/v2/city/lookup?location=${longitide},${latitude}$key=${key}',
success(res){
console.log(res.data)
that.setData({
  shi:res.data.location[0].adm2,
  xian:res.data.location[0].name
})
}
})
wx.request({
  url: 'https://devapi.qweather.com/v7/weather/now?location=${longitide},${latitude}$key=${key}',
  success(res){
  console.log(res.data)
  that.setData({
    temp:res.data.now.temp,
    tianqi:res.data.now.text
  })
  }
  })
}
})
},

  onClickPowerInfo(e) {
    const index = e.currentTarget.dataset.index;
    const powerList = this.data.powerList;
    powerList[index].showItem = !powerList[index].showItem;
    if (powerList[index].title === '数据库' && !this.data.haveCreateCollection) {
      this.onClickDatabase(powerList);
    } else {
      this.setData({
        powerList
      });
    }
  },

  onChangeShowEnvChoose() {
    wx.showActionSheet({
      itemList: this.data.envList.map(i => i.alias),
      success: (res) => {
        this.onChangeSelectedEnv(res.tapIndex);
      },
      fail (res) {
        console.log(res.errMsg);
      }
    });
  },

  onChangeSelectedEnv(index) {
    if (this.data.selectedEnv.envId === this.data.envList[index].envId) {
      return;
    }
    const powerList = this.data.powerList;
    powerList.forEach(i => {
      i.showItem = false;
    });
    this.setData({
      selectedEnv: this.data.envList[index],
      powerList,
      haveCreateCollection: false
    });
  },

  jumpPage(e) {
    wx.navigateTo({
      url: `/pages/${e.currentTarget.dataset.page}/index?envId=${this.data.selectedEnv.envId}`,
    });
  },

  onClickDatabase(powerList) {
    wx.showLoading({
      title: '',
    });
    wx.cloud.callFunction({
      name: 'quickstartFunctions',
      config: {
        env: this.data.selectedEnv.envId
      },
      data: {
        type: 'createCollection'
      }
    }).then((resp) => {
      if (resp.result.success) {
        this.setData({
          haveCreateCollection: true
        });
      }
      this.setData({
        powerList
      });
      wx.hideLoading();
    }).catch((e) => {
      console.log(e);
      this.setData({
        showUploadTip: true
      });
      wx.hideLoading();
    });
  }
});
