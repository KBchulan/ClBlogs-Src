package main

import (
	"fmt"
	"ginLearn/routes"
	"net/http"
	"path/filepath"
	"strconv"

	"github.com/gin-gonic/gin"
)

func GetPing(c *gin.Context) {
	c.String(200, "Hello, world")
}

func GetQuery(c *gin.Context) {
	id := c.Query("id")
	ageStr := c.DefaultQuery("age", "18")
	age, _ := strconv.Atoi(ageStr)
	c.String(200, "Receive id is: %s, age is: %d", id, age)
}

func GetDynamicUrl(c *gin.Context) {
	uid := c.Param("uid")
	c.String(200, "Receive uid is: %s", uid)
}

func ReturnBasic(c *gin.Context) {
	c.Writer.WriteHeader(200)

	c.Writer.Header().Set("X-Custom-Header", "my-value")
	c.Writer.Header().Set("Cache-Control", "no-cache")
	c.Writer.Header().Set("Server", "MyServer/1.0")

	c.Writer.WriteString("Hello")
}

func ReturnString(c *gin.Context) {
	aid := c.Query("aid")
	c.String(200, "aid=%s", aid)
}

func ReturnJsonSimple(c *gin.Context) {
	name := c.Query("name")
	age := c.Query("age")
	c.JSON(http.StatusOK, gin.H{
		"name":    name,
		"age":     age,
		"message": "Received",
	})
}

func ReturnJsonByStruct(c *gin.Context) {
	name := c.Query("name")
	age, _ := strconv.Atoi(c.Query("age"))

	var person = Person{
		Name: name,
		Age:  age,
	}
	c.JSON(http.StatusOK, person)
}

func ReturnXMLSimple(c *gin.Context) {
	c.XML(http.StatusOK, gin.H{
		"message": "hello, world",
	})
}

func ReturnXMLByStruct(c *gin.Context) {
	var person = Person{
		Name: "chu",
		Age:  20,
	}
	c.XML(http.StatusOK, person)
}

func ReturnHtml1(c *gin.Context) {
	c.HTML(http.StatusOK, "default/index.html", gin.H{})
}

func ReturnHtml2(c *gin.Context) {
	c.HTML(http.StatusOK, "admin/index.html", gin.H{})
}

func ReturnOutData(c *gin.Context) {
	person := Person{
		Name: "lan",
		Age:  20,
	}
	c.HTML(http.StatusOK, "default/index.html", gin.H{
		"title":  "这个是标题",
		"person": person,
		"gender": "man",
	})
}

func PostForm(c *gin.Context) {
	username := c.PostForm("username")
	password := c.PostForm("password")
	age := c.DefaultPostForm("age", "20")

	c.JSON(200, gin.H{
		"username": username,
		"password": password,
		"age":      age,
	})
}

func PostShould(c *gin.Context) {
	type UserInfo struct {
		Name  string `json:"name" xml:"name"`
		Age   int    `json:"age" xml:"age"`
		Email string `json:"email" xml:"email"`
	}

	var user UserInfo
	err := c.ShouldBind(&user)
	if err == nil {
		c.JSON(http.StatusOK, gin.H{
			"name":  user.Name,
			"age":   user.Age,
			"email": user.Email,
		})
	} else {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": err.Error(),
		})
	}
}

func InitMiddleware(c *gin.Context) {
	c.String(200, "我是一个中间件")
}

func NextUse(c *gin.Context) {
	c.Writer.WriteString("正在执行中间件1")
	c.Next()
	c.Writer.WriteString("控制权回来了")
}

func UploadFile(c *gin.Context) {
	file, err := c.FormFile("file")
	username := c.PostForm("username")

	if err != nil {
		c.String(http.StatusBadRequest, "upload file error: %s", err.Error())
		return
	}

	uploadPath := "./upload"
	dst := filepath.Join(uploadPath, file.Filename)

	err2 := c.SaveUploadedFile(file, dst)
	if err2 != nil {
		c.String(http.StatusBadRequest, "save file error: %s", err2.Error())
		return
	}
	c.JSON(http.StatusOK, gin.H{
		"message":  fmt.Sprintf("%s file upload success", file.Filename),
		"username": username,
	})
}

func UploadManyFile(c *gin.Context) {
	form, err := c.MultipartForm()

	if err != nil {
		c.String(http.StatusBadRequest, "解析表单失败: %s", err.Error())
		return
	}

	// 普通字段
	userNames := form.Value["username"]

	// 文件
	files := form.File["files"]
	for _, v := range files {
		dst := filepath.Join("./upload", v.Filename)
		if err := c.SaveUploadedFile(v, dst); err != nil {
			c.String(http.StatusInternalServerError, "保存文件 '%s' 失败: %v", v.Filename, err)
			return
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"username": userNames[0],
		"message":  "file upload success",
	})
}

func SetCookie(c *gin.Context) {
	c.SetCookie("my-cookie", "hello cookie", 3600, "/", "localhost", false, true)
	c.String(200, "Cookie set successful")
}

func GetCookie(c *gin.Context) {
	cookie, err := c.Cookie("my-cookie")
	if err != nil {
		c.String(http.StatusBadRequest, "This cookie deleted")
	}
	c.String(http.StatusOK, cookie)
}

func DeleteCookie(c *gin.Context) {
	c.SetCookie("my-cookie", "", -1, "/", "localhost", false, true)
	c.String(200, "Cookie delete success")
}

func main() {
	// Debug 模式会有很多信息，如路由 -> 回调，不好看
	gin.SetMode(gin.ReleaseMode)
	r := gin.Default()

	// 注册路由

	// 基本的三个，如 查询参数，动态路由
	r.GET("/get-ping", GetPing)
	r.GET("/get-query", GetQuery)
	r.GET("/get-dynamic-url/:uid", GetDynamicUrl)

	// 基本返回
	r.GET("/return-basic", ReturnBasic)

	// 返回普通字符串
	r.GET("/return-string", ReturnString)

	// 返回 Json
	r.GET("/return-json-simple", ReturnJsonSimple)
	r.GET("/return-json-by-struct", ReturnJsonByStruct)

	// 返回 XML
	r.GET("/return-xml-simple", ReturnXMLSimple)
	r.GET("/return-xml-by-struct", ReturnXMLByStruct)

	// 返回 HTML 的模板渲染
	r.LoadHTMLGlob("templates/**/*")
	r.GET("/return-html1", ReturnHtml1)
	r.GET("/return-html2", ReturnHtml2)

	// Gin 模板用法
	r.GET("return-template-use", ReturnOutData) // 输出数据

	// 静态文件
	r.Static("/static", "./static")

	// post 表单数据
	r.POST("post-form", PostForm)
	r.POST("post-should", PostShould)

	// 路由分组
	v1 := r.Group("/v1")
	{
		v1.GET("/login", func(ctx *gin.Context) {
			ctx.String(200, "Hello, World")
		})
	}
	routes.AdminRoutesInit(r)

	// mvc 实例接口
	routes.UserRoutesInit(r)

	// 中间件
	r.GET("/middleware_init", InitMiddleware, func(ctx *gin.Context) {
		ctx.String(200, "首页")
	})

	r.GET("/next-use", NextUse, func(ctx *gin.Context) {
		ctx.Status(200)
		ctx.Writer.WriteString("主要函数")
	})

	// 文件上传
	r.POST("/upload", UploadFile)
	r.POST("/upload-more", UploadManyFile)

	// cookie操作
	r.GET("/set-cookie", SetCookie)
	r.GET("/get-cookie", GetCookie)
	r.GET("/delete-cookie", DeleteCookie)

	// 启动服务
	fmt.Println("Server starting at http://localhost:5000")
	err := r.Run("localhost:5000")
	if err != nil {
		fmt.Printf("Error start server, msg is: %s\n", err)
	}
}

type Person struct {
	Name string `json:"name"`
	Age  int    `json:"age"`
}
