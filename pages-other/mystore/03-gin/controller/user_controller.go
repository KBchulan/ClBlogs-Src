package controller

import (
	"ginLearn/service"
	"net/http"

	"github.com/gin-gonic/gin"
)

type UserController struct {
	userService *service.UserService
}

func NewUserController(service *service.UserService) *UserController {
	return &UserController{userService: service}
}

func (uc *UserController) GetUserByName(c *gin.Context) {
	name := c.Query("name")
	if name == "" {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": "name can't empty",
		})
		return
	}

	if name != "chulan" {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": "this user doesn't exist",
		})
		return
	}

	user, err := uc.userService.GetUserByName(name)

	if err == nil {
		c.JSON(http.StatusOK, *user)
	} else {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": err.Error(),
		})
	}
}
