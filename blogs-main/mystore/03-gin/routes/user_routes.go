package routes

import (
	"ginLearn/controller"
	"ginLearn/service"

	"github.com/gin-gonic/gin"
)

func UserRoutesInit(r *gin.Engine) {
	userService := service.NewUserService()
	userController := controller.NewUserController(userService)

	user := r.Group("/user")
	{
		user.GET("/get-user", userController.GetUserByName)
	}
}
