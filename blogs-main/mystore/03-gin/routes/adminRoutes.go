package routes

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

func AdminRoutesInit(r *gin.Engine) {
	adminRouter := r.Group("/admin")
	{
		adminRouter.GET("/user", func(ctx *gin.Context) {
			ctx.String(http.StatusOK, "hello, user")
		})
	}
}
