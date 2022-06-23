//index.js
const app = getApp()

Page({
  data: {
    avatarUrl: './user-unlogin.png',
    userInfo: {},
    logged: false,
    takeSession: false,
    requestResult: '',

  },
  radioChange: function (e) {
    var str = null;
    for (var value of this.data.items) {
      if (value.name === e.detail.value) {
        str = value.value;
        break;
      }
    }
    this.setData({ radioStr: str });
  },
  points:function(e) {
    var that = this
    wx.request({
      url: 'http://api.heclouds.com/devices/860653282',
      //设备ID
      //api-key
      header:{
        "api-key":"uCA2FSqcnjbv6F4q9siFbtw0di4="
      },
      data:{
        limit:1
      },
      method :"GET",
      success:function(res){
       that.setData({
         shidu:res.data.data.datastreams[0].datapoints[0].value,
         wendu:res.data.data.datastreams[1].datapoints[0].value,
       })    
      }
    })
  },
  onShow: function(){
 
  },
 
  onLoad: function() {
    
  },

})
