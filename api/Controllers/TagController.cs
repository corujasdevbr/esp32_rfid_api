using Microsoft.AspNetCore.Mvc;

namespace esp32rfi.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class TagController : ControllerBase
    {
        [HttpPost]
        public async Task<IActionResult> Post([FromBody] DTO.TagDto request, CancellationToken cancellationToken)
        {
            if(request.Tag == " a1 07 66 1b")
            {
                return Ok(new
                {
                    tag = request.Tag,
                    valido = true
                });
            }
            else
            {
                return BadRequest(new
                {
                    tag = request.Tag,
                    valido = false
                });
            }
        }
    }
}